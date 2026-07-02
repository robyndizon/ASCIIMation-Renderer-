/**
 * \file display-test.cpp
 * \author CS 70 Provided Code
 *
 * \brief Provides a simple demo of the Display class to confirm it works
 *
 */

#include "display.hpp"
#include <string>
#include <iostream>
#include <unistd.h>  // For usleep

int ncurses_demo() {
    constexpr int WIDTH = 80;
    constexpr int HEIGHT = 24;
    constexpr int FRAME_USECS = 100000;  // 0.1 seconds per frame
    int keyPressed;                      // To hold keypresses

    Display display{WIDTH, HEIGHT};

    // Draw a border around the edges
    for (int col = 0; col < WIDTH; ++col) {
        display.putCharAt(col, 0, '#');
        display.putCharAt(col, HEIGHT - 1, '#');
    }
    for (int row = 0; row < HEIGHT; ++row) {
        display.putCharAt(0, row, '#');
        display.putCharAt(WIDTH - 1, row, '#');
    }
    display.refresh();

    // Slowly draw a diagonal line
    for (int i = 1; i < HEIGHT - 1 && i < WIDTH - 1; ++i) {
        display.refresh();
        display.putCharAt(i, i, '*');
        usleep(FRAME_USECS);
        keyPressed = display.checkForKeypress();
        if (keyPressed != Display::NO_KEY_PRESSED) {
            return keyPressed;  // Return the key that was pressed
        }
    }

    // Say "Hello, World!" in the center
    std::string message = "Hello, World!";
    int startCol = (WIDTH - message.size()) / 2;
    int startRow = HEIGHT / 2;
    for (size_t i = 0; i < message.size(); ++i) {
        display.putCharAt(startCol + i, startRow, message[i]);
    }
    display.refresh();

    // Pause to let the user see the unless they press a key
    for (int i = 0; i < 50; ++i) {
        keyPressed = display.checkForKeypress();
        if (keyPressed != Display::NO_KEY_PRESSED) {
            return keyPressed;  // Return the key that was pressed
        }
        usleep(FRAME_USECS);
    }
    return display.checkForKeypress();  // Final check for keypress
}

int main() {
    std::cout << "Starting ncurses demo. Press any key to exit early.\n";
    int c = ncurses_demo();
    if (c == Display::NO_KEY_PRESSED) {
        std::cout << "No key was pressed.\n";
    } else if (c < ' ' || c > '~') {
        std::cout << "You pressed a non-printable key (code " << c << ").\n";
    } else {
        std::cout << "You pressed the '" << char(c) << "' key.\n";
    }
    return 0;
}
