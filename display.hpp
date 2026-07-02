/**
 * \file display.hpp
 * \author CS 70 Provided Code
 *
 * \brief Interface definition for the Display class, which manages
 *        the terminal display for ASCIImation.
 *
 */

#ifndef DISPLAY_HPP_INCLUDED
#define DISPLAY_HPP_INCLUDED

#include <cstddef>
#include <csignal>
#include <cassert>
#include <string>
#include <stdexcept>
#include <atomic>
#ifdef DISPLAY_OUTPUT_FILE
#include <fstream>
#include <chrono>
#endif

/**
 * \class Display
 * \brief Manages the terminal display using ncurses
 *
 * \details
 * Encapsulates all ncurses functionality to provide a simple interface
 * for displaying characters on the terminal. Handles initialization,
 * cleanup, and drawing operations.
 *
 * \warning Only one Display instance may exist at a time.
 */
class Display {
 public:
    /**
     * \brief Constant indicating no key was pressed
     */
    static constexpr int NO_KEY_PRESSED = -1;

    /**
     * \brief Maximum allowed display dimensions
     */
    static constexpr int MAX_WIDTH = 132;
    static constexpr int MAX_HEIGHT = 64;

    /**
     * \brief Constructs and initializes the display
     * \post The terminal is set up for animation display
     */
    Display(int width = 80, int height = 24);

    /**
     * \brief Destructor cleans up the display
     * \post The terminal is restored to normal mode
     */
    ~Display();

    // Disable copy constructor and assignment operator
    Display(const Display&) = delete;
    Display& operator=(const Display&) = delete;

    /**
     * \brief Clears the entire display
     * \post All positions on the display contain spaces
     */
    void clear();

    /**
     * \brief Places a character at a specific position
     * \param col The column position (0 to DISPLAY_WIDTH-1)
     * \param row The row position (0 to DISPLAY_HEIGHT-1)
     * \param ch The character to display
     * \warning Does nothing if row or col are out of bounds
     */
    void putCharAt(size_t col, size_t row, char ch);

    /**
     * \brief Updates the screen to show all pending changes
     * \post All characters placed with putCharAt are visible
     */
    void refresh();

    /**
     * \brief Checks if the user has pressed a key
     * \returns The character pressed, or -1 if no key was pressed
     */
    int checkForKeypress();

    /**
     * \brief Gets the width of the display
     * \returns The display width in characters
     */
    int getWidth() const;

    /**
     * \brief Gets the height of the display
     * \returns The display height in characters
     */
    int getHeight() const;

 private:
    // Singleton instance pointer for signal handlers
    static Display* theDisplayInstance_;

#ifndef DISPLAY_OUTPUT_FILE
    // Flag to indicate if a resize event occurred
    std::atomic<bool> resized_{false};

    // Internal functions for ncurses management
    void processResizeEvent();
    static void shutdownNcurses();
    static void setSignalHandlers(sig_t crash, sig_t resize);
    static void resizeHandler(int signum);
    static void cleanUpScreenHandler(int signum);
    static void startupNcurses();
#else
    // Max runtime in seconds (override with ASCIIMATION_MAX_RUNTIME env var)
    float max_runtime_ = 5.0f;
    // Max frames to write (override with ASCIIMATION_MAX_FRAMES env var)
    int max_frames_ = 500;
    // File output mode - no ncurses, just write to a file
    std::ofstream outputFile_;
    // High-resolution timestamp of the start of the display
    std::chrono::high_resolution_clock::time_point startTime_;
    // Number of frames written so far
    int frameCount_ = 0;
    // Dimensions of the display
    int width_;
    int height_;
    // Buffer for the current frame
    std::string frameBuffer_[MAX_HEIGHT];

    // Helpers for file output mode
    float elapsedTime() const;
    void writeFrameToFile();
#endif
};

#endif  // DISPLAY_HPP_INCLUDED
