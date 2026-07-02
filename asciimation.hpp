/**
 * \file asciimation.hpp
 * \author CS 70 Provided Code
 *
 * \brief Provides the interface definition for the Asciimation class which
 *        manages the overall animation, including multiple sprites.  It
 *        stores the current frame and ensures it is sent to the display.
 *
 */

#ifndef ASCIIMATION_HPP_INCLUDED
#define ASCIIMATION_HPP_INCLUDED

#include "display.hpp"
#include "sprite.hpp"
#include "boundingbox.hpp"
#include "numsprites.hpp"
#include "scene.hpp"

#include <string>

/**
 * \class Asciimation
 * \brief Manages an ASCIImation movie with multiple sprites
 *
 * \details
 * Stores the current frame of the movie as a character array, and
 * manages the individual sprites that make up the movie. It handles
 * the rendering of these sprites onto the frame and ensures the
 * correct frame is displayed at the right time.
 *
 */
class Asciimation {
 public:
    static const int MOVIE_WIDTH = 80;
    static const int MOVIE_HEIGHT = 24;

    /**
     * \brief Creates an Asciimation object with a blank image
     */
    Asciimation();

    // Disable copy constructor and assignment operator
    Asciimation(const Asciimation& other) = delete;
    Asciimation& operator = (const Asciimation& other) = delete;

    /**
     * \brief Displays an asciimation on the screen until the user enters "q"
     * \post The screen is destroyed.
     */
    void play(Scene& Scene);

 private:
    BoundingBox movieScreenBounds_;  // The bounds of the movie on the screen

    // The characters to display on the screen.
    char frame_[MOVIE_WIDTH * MOVIE_HEIGHT];

    void clearFrame();                        // Clear the character array
    void renderSprite(const Sprite& sprite);  // Render a sprite into the frame
    void copyToScreen(Display& display) const;  // Copy array to display
};

#endif  // ASCIIMATION_HPP_INCLUDED
