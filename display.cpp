/**
 * \file display.cpp
 * \author CS 70 Provided Code
 *
 * \brief Implements the Display class
 *
 */

#include "display.hpp"
#include <string>
#include <iostream>

// We have essentially two modes: normal ncurses mode, and
// file output mode (for environments that don't support ncurses).
// The DISPLAY_OUTPUT_FILE macro is defined at compile time to
// enable file output mode and specify the output file name.
//
// The implementations are utterly different, but the interface is the same.

#ifndef DISPLAY_OUTPUT_FILE

#include <ncurses.h>
#include <unistd.h>
#include <csignal>
#include <cassert>
#include <cstring>  // for strsignal
#include <sys/ioctl.h>

// ncurses officially defines ERR as a negative value, but doesn't
// guarantee that it's always -1, but in practice it is.  We use -1 in the
// header to avoid a dependency on ncurses in code that uses display.hpp
static_assert(ERR == -1, "ERR value has changed! Update NO_KEY_PRESSED.");

Display::Display(int width, int height) {
    if (theDisplayInstance_ != nullptr) {
        throw std::runtime_error("Only one Display instance allowed");
    }

    // Sanity check the requested dimensions
    if (width <= 0 || width > MAX_WIDTH || height <= 0 || height > MAX_HEIGHT) {
        throw std::invalid_argument("Invalid display dimensions");
    }

    // Before we even try to start ncurses, sanity check the terminal
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        throw std::runtime_error(
            "Couldn't get terminal size, is this program running in a "
            "terminal?");
    }
    // Just warn to cerr if the terminal is smaller than requested
    if (w.ws_col < width || w.ws_row < height) {
        std::cerr << "Warning: Terminal is smaller than requested display size "
                  << width << "x" << height << ". Actual size is " << w.ws_col
                  << "x" << w.ws_row << ".\n";
        std::cerr << "Attempting resize...\n";
        // Try to resize the terminal window using ANSI escape codes
        std::cout << "\033[8;" << height << ";" << width << "t" << std::flush;
        // Wait a moment to let the terminal resize
        usleep(500000);
        // Re-check the terminal size
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
            throw std::runtime_error(
                "Couldn't get terminal size, is this program running in a "
                "terminal?");
        }
        if (w.ws_col < width || w.ws_row < height) {
            std::cerr
                << "Warning: Terminal is still smaller than requested size "
                << width << "x" << height << ". Actual size is " << w.ws_col
                << "x" << w.ws_row << ".\n";
            std::cerr << "Continuing anyway...\n";
        } else {
            std::cerr << "Resize successful, new size is " << w.ws_col << "x"
                      << w.ws_row << ".\n";
        }
    }

    theDisplayInstance_ = this;
    startupNcurses();
}

Display::~Display() {
    assert(theDisplayInstance_ == this);
    shutdownNcurses();
    theDisplayInstance_ = nullptr;
}

void Display::clear() {
    processResizeEvent();
    // Clear the entire display with spaces
    size_t height = getHeight();
    size_t width = getWidth();
    for (size_t row = 0; row < height; ++row) {
        for (size_t col = 0; col < width; ++col) {
            mvaddch(row, col, ' ');
        }
    }
}

void Display::putCharAt(size_t col, size_t row, char ch) {
    processResizeEvent();

    // Ncurses will reject out-of-bounds coordinates, so just pass them through
    // without doing any checking here.
    mvaddch(row, col, ch);
}

void Display::refresh() {
    processResizeEvent();
    ::refresh();  // Call the ncurses refresh function
}

int Display::checkForKeypress() {
    processResizeEvent();
    return getch();
}

int Display::getWidth() const {
    // Use ncurses COLS global variable for current width
    return COLS;
}

int Display::getHeight() const {
    // Use ncurses LINES global variable for current height
    return LINES;
}

void Display::shutdownNcurses() {
    // Restore the saved shell terminal before exiting.
    // Put the cursor back to beginning of the last row of the terminal, where
    // we expect it to be when the program exits.
    mvcur(0, COLS - 1, LINES - 1, 0);
    endwin();
    setSignalHandlers(SIG_DFL, SIG_DFL);
}

/*
 * Singleton instance pointer for signal handlers
 */
Display* Display::theDisplayInstance_ = nullptr;

/*
 * Internal functions to handle window resize events
 */
void Display::resizeHandler(int) {
    assert(theDisplayInstance_ != nullptr);
    theDisplayInstance_->resized_.store(true);
}

void Display::processResizeEvent() {
    if (resized_.load()) {
        // Handle the resize event (e.g., adjust the display)
        resized_.store(false);
        endwin();
        refresh();
    }
}

/*
 * Internal function to handle signals and clean up the screen
 */
void Display::cleanUpScreenHandler(int signum) {
    assert(theDisplayInstance_ != nullptr);
    shutdownNcurses();

    std::string errorName = strsignal(signum);
    if (errorName.empty()) {
        errorName = "Mystery signal";
    }
    std::cerr << "Received signal (" << signum << " - " << errorName
              << ") received.\n";

    _exit(signum);
}

void Display::setSignalHandlers(sig_t crash, sig_t resize) {
    for (int sig :
         {SIGSEGV, SIGFPE, SIGINT, SIGTERM, SIGILL, SIGBUS, SIGQUIT, SIGABRT}) {
        signal(sig, crash);
    }
    signal(SIGWINCH, resize);
}

void Display::startupNcurses() {
    // Make sure the screen gets cleaned up if we exit abnormally
    setSignalHandlers(cleanUpScreenHandler, resizeHandler);

    // Initialize the screen
    initscr();

    // Don't echo characters entered by the user
    noecho();

    // Make the cursor invisible
    curs_set(0);

    // Tell ncurses not to block waiting for user input on getch() calls
    nodelay(stdscr, TRUE);

    // We're not using the cursor, so tell ncurses to leave it wherever our
    // updates happen to put it
    leaveok(stdscr, TRUE);

    // Leave the cursor on the bottom line of the screen instead of scrolling
    // if one of our updates would put it off-screen
    scrollok(stdscr, FALSE);
}

#else  // DISPLAY_OUTPUT_FILE

#include <fstream>
#include <chrono>
#include <iomanip>
#include <stdexcept>
#include <charconv>
#include <cstdlib>
#include <cstring>
#include <cassert>

Display::Display(int width, int height)
    : outputFile_{DISPLAY_OUTPUT_FILE}, width_{width}, height_{height} {
    if (theDisplayInstance_ != nullptr) {
        throw std::runtime_error("Only one Display instance allowed");
    }

    // Sanity check the requested dimensions
    if (width <= 0 || width > MAX_WIDTH || height <= 0 || height > MAX_HEIGHT) {
        throw std::invalid_argument("Invalid display dimensions");
    }

    if (!outputFile_.is_open()) {
        throw std::runtime_error("Couldn't open output file for writing");
    }

    // Check the environment for ASCIIMATION_MAX_RUNTIME and
    // ASCIIMATION_MAX_FRAMES settings
    const char* env_runtime = std::getenv("ASCIIMATION_MAX_RUNTIME");
    if (env_runtime != nullptr) {
        float val = 0.0f;
        auto [ptr, ec] = std::from_chars(
            env_runtime, env_runtime + strlen(env_runtime), val);
        if (ec == std::errc() && val > 0.0f) {
            max_runtime_ = val;
        } else {
            std::cerr
                << "Warning: Ignoring invalid ASCIIMATION_MAX_RUNTIME value: "
                << env_runtime << "\n";
        }
    }
    const char* env_frames = std::getenv("ASCIIMATION_MAX_FRAMES");
    if (env_frames != nullptr) {
        int val = 0;
        auto [ptr, ec] =
            std::from_chars(env_frames, env_frames + strlen(env_frames), val);
        if (ec == std::errc() && val > 0) {
            max_frames_ = val;
        } else {
            std::cerr
                << "Warning: Ignoring invalid ASCIIMATION_MAX_FRAMES value: "
                << env_frames << "\n";
        }
    }

    std::cerr << "Generating movie." << std::flush;

    // Print the header for the output file
    outputFile_ << "ASCIImation: " << width_ << " x " << height_ << "\n";

    theDisplayInstance_ = this;
    startTime_ = std::chrono::high_resolution_clock::now();
    clear();
}

Display::~Display() {
    assert(theDisplayInstance_ == this);
    writeFrameToFile();
    outputFile_.close();
    theDisplayInstance_ = nullptr;
    std::cerr << " done!\nMovie complete. Output written to "
              << DISPLAY_OUTPUT_FILE << std::endl;
}

void Display::clear() {
    // Clear the entire display with spaces
    for (int row = 0; row < height_; ++row) {
        frameBuffer_[row] = std::string(width_, ' ');
    }
}

void Display::putCharAt(size_t col, size_t row, char ch) {
    if (row < 0 || row >= static_cast<size_t>(height_) || col < 0
        || col >= static_cast<size_t>(width_)) {
        // Out of bounds, do nothing
        return;
    }
    frameBuffer_[row][col] = ch;
}

void Display::refresh() {
    // Extreme measures if we want to quit but haven't yet
    auto elapsed = elapsedTime();
    if (elapsed > max_runtime_ * 1.2 || frameCount_ >= max_frames_) {
        throw std::runtime_error(
            "Exceeded maximum runtime or frame count without quitting");
    }

    writeFrameToFile();
    std::cerr << "." << std::flush;  // Progress indicator
}

float Display::elapsedTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime_)
            .count();
    return elapsed / 1000.0f;
}

int Display::checkForKeypress() {
    // We send a 'q' to quit after max_runtime_ seconds or max_frames_ frames
    auto elapsed = elapsedTime();
    if (elapsed > max_runtime_ || frameCount_ >= max_frames_) {
        return 'q';
    }
    return NO_KEY_PRESSED;
}

int Display::getWidth() const {
    return width_;
}

int Display::getHeight() const {
    return height_;
}

void Display::writeFrameToFile() {
    // Get the elapsed time since the start of the display
    auto elapsed = elapsedTime();

    std::string separator(width_, '=');

    // Write a timestamp and the frame to the output file
    outputFile_ << "Frame " << frameCount_++ << " at " << std::fixed
                << std::setprecision(3) << elapsed << " seconds:\n"
                << separator << "\n";
    for (int row = 0; row < height_; ++row) {
        outputFile_ << frameBuffer_[row] << '\n';
    }
    outputFile_ << separator << "\n";
    outputFile_.flush();
}

/*
 * Singleton instance pointer for signal handlers
 */
Display* Display::theDisplayInstance_ = nullptr;

#endif
