/**
 * \file numsprites.hpp
 * \author CS 70 Provided Code
 *
 * \brief Defines the number of sprites used in the asciimation.
 *
 * \details
 * This file defines a constant for the number of sprites used in the
 * asciimation program. It is included in multiple source files to ensure
 * consistency.
 *
 * This constant is necessary because we are using fixed-size arrays
 * to hold the sprites, and the size of these arrays must be known at
 * compile time.
 */

#ifndef NUMSPRITES_HPP_INCLUDED
#define NUMSPRITES_HPP_INCLUDED

/**
 * \brief The number of sprites in the asciimation
 */
constexpr int NUM_SPRITES = 3;  // For future expansion

#endif  // NUMSPRITES_HPP_INCLUDED
