/**
 * \file sprite.hpp
 * \author CS 70 Provided Code
 *
 * \brief Interface definition for the Sprite class
 *
 * \details Represents a single ASCIImation sprite, including its position,
 * velocity, and character data.
 */

#ifndef SPRITE_HPP_INCLUDED
#define SPRITE_HPP_INCLUDED

#include <iostream>
#include <fstream>
#include <string>
#include <utility>  // actually unnecessary but CPPLINT (incorrectly) wants it
#include "boundingbox.hpp"

/**
 * \class Sprite
 * \brief Holds the contents of a single ASCIImation sprite
 *
 * \details
 * Stores an array of characters that will be displayed to the screen, along
 * with the sprite's current location and velocity. Sprites can have variable
 * dimensions up to the maximum size.
 *
 */
class Sprite {
 public:
    // Maximum sprite dimensions
    static constexpr int MAX_SPRITE_DATA = 800;  // for fixed-size array

    /**
     * \brief Reads a text file containing the characters for a sprite
     * \param fname The filename to read the sprite from
     * \param x Initial x position of the sprite
     * \param y Initial y position of the sprite
     * \post The sprite's character array is populated with data from the file
     */
    Sprite(std::string fname, int x, int y);

    /**
     * \brief Reads an istream containing the characters for a sprite
     * \param input The input stream to read the sprite from
     * \param x Initial x position of the sprite
     * \param y Initial y position of the sprite
     * \post The sprite's character array is populated with data from the stream
     */
    Sprite(std::istream& input, int x, int y);

    // Disable the default, copy, and assignment constructor
    Sprite() = delete;
    Sprite(const Sprite& other) = delete;
    Sprite& operator = (const Sprite& other) = delete;

    /**
     * \brief Gets the Sprite's bounding box 
     * \returns The Sprite's bounding box
     */
    BoundingBox getBoundingBox() const;

    /**
     * \brief Gets the char at the specified position in the Sprite char array 
     * \param x The x-coordinate of the point relative to the left of the Sprite
     * \param y The y-coordinate of the point relative to the top of the Sprite 
     * \returns The character at the point (x, y) is inside the Sprite
     */
    char getCharAt(int x, int y) const;

    /**
     * \brief Moves Sprite  
     * \param x The x-coordinate of the point of the Sprite's new position 
     * \param y The y-coordinate of the point of the Sprite's new position 
     */
    void setPosition(int x, int y);

    /**
     * \brief Sets velocity of Sprite
     * \param x The x-component of the Sprite's velocity 
     * \param y The y-component of the Sprite's velocity 
     */
    void setVelocity(int vx, int vy);

    /**
     * \brief Updates the Sprite's position based on velocity 
     * \param screenBounds The size of the screen and should be a const reference
     */
    void move(const BoundingBox& screenBounds);

 private:
    // The sprite's position and size
    BoundingBox bounds_;

    // Velocity in pixels per frame
    int velocityX_;
    int velocityY_;

    int contents_[MAX_SPRITE_DATA];

    // Helper function to read sprite data from a stream
    void readFromStream(std::istream& input);
};

#endif  // SPRITE_HPP_INCLUDED
