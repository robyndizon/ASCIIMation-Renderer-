/**
 * \file asciimation.cpp
 * \author CS 70 Provided Code
 *
 * \brief Implements the Asciimation class
 *
 */

#include "display.hpp"
#include "asciimation.hpp"
#include "sprite.hpp"
#include "scene.hpp"

#include <iostream>
#include <unistd.h>

Asciimation::Asciimation()
: movieScreenBounds_{0, 0, MOVIE_WIDTH, MOVIE_HEIGHT} {
    // Nothing else to do here
}

void Asciimation::play(Scene& scene) {
    // Time to sleep between screen refreshes
    const int PAUSE_TIME_IN_MICROSECONDS = 40000;

    Display display{MOVIE_WIDTH, MOVIE_HEIGHT};

    // Play the movie by refreshing the screen with new data, every so often
    int key = 0;
    bool finished = false;
    int numSprites = scene.getNumSprites();

    while (!finished) {
        clearFrame();

        // Render all sprites
        for (int i = 0; i < numSprites; ++i) {
            renderSprite(scene.getSprite(i));
        }

        // Show the frame on the screen
        copyToScreen(display);
        display.refresh();

        // Sleep between updates
        usleep(PAUSE_TIME_IN_MICROSECONDS);

        // Check to see if the user has entered 'q' to quit
        key = display.checkForKeypress();
        if (key == 'q') {
            finished = true;
        }

        // Update the scene for the next frame
        scene.advance(movieScreenBounds_);
    }
}

void Asciimation::clearFrame() {
    // Clear the frame by setting all characters to spaces
    for (size_t i = 0; i < MOVIE_HEIGHT * MOVIE_WIDTH; ++i) {
        frame_[i] = ' ';
    }
}

void Asciimation::renderSprite(const Sprite& sprite) {
    // Loop through all of the characters in the sprite and copy them to
    // the right spot in the movie's frame character array
    BoundingBox spriteBounds = sprite.getBoundingBox();
    spriteBounds
        = spriteBounds.findIntersection(movieScreenBounds_);
    int locationofSpriteTopLeft
        = spriteBounds.y_ * MOVIE_WIDTH + spriteBounds.x_;
    size_t spriteWidth = spriteBounds.width_;
    size_t spriteHeight = spriteBounds.height_;
    for (size_t i = 0; i <  spriteWidth; ++i) {
        for (size_t j = 0; j < spriteHeight; ++j) {
            frame_[locationofSpriteTopLeft + i + j * MOVIE_WIDTH] =
                sprite.getCharAt(i, j);
        }
    }
}

void Asciimation::copyToScreen(Display& display) const {
    // Loop through all of the characters in the movie's character array and
    // copy them to the right place on the screen
    // Careful: display.putCharAt wants screen coordinates, and frame_
    // is indexed by movie coordinates.  Usually the screen and movie
    // coordinates are the same, but you can imagine a 4:3 display showing
    // a widescreen movie, in which case the movie would be centered on
    // the screen.  In other words, add the movieBounds_ x and y to the
    // coordinates you pass to display.putCharAt.

    size_t displayX;
    size_t displayY;

    // Nested loop through all chars in movie, putting them in the display
    for (size_t i = 0; i < MOVIE_WIDTH; ++i) {
        for (size_t j = 0; j < MOVIE_HEIGHT; ++j) {
            displayX = movieScreenBounds_.x_ + i;
            displayY = movieScreenBounds_.y_ + j;
            display.putCharAt(displayX, displayY, frame_[i + j * MOVIE_WIDTH]);
        }
    }
}
