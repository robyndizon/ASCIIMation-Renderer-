/**
 * \file scence.hpp
 * \author Leah and Robyn
 *
 * \brief Interface definition for the Scene class
 *
 * \details Represents a single ASCIImation scene, including its sprites,
 * position, and velocity.
 */

#ifndef SCENE_HPP_INCLUDED
#define SCENE_HPP_INCLUDED

#include "boundingbox.hpp"
#include "numsprites.hpp"
#include "sprite.hpp"

/**
 * \class Scene
 * \brief Holds the contents of a single ASCIImation scene
 *
 * \details
 * Stores an array of characters that will be displayed to the screen, along
 * with the scene's current location and velocity. 
 *
 */
class Scene {
 public:
    // Sprites in scene
    static constexpr int SCENE_NUM_SPRITES = NUM_SPRITES;

    /**
     * \brief Creates a scene
     * \post The scene's sprite array is populated
     */
    Scene();


    // Disable the copy constructor, and assignment constructor
    Scene(const Scene& other) = delete;
    Scene& operator = (const Scene& other) = delete;

    /**
     * \brief Gets the Sprite at index i
     * \returns The Sprite at index i
     */
    const Sprite& getSprite(int i) const;

    /**
     * \brief Gets the number of sprites in the scene
     * \returns The number of sprites in the scene
     */
    int getNumSprites() const;


    /**
     * \brief Advances the scene by one frame, moving all sprites
     * \param The key that the user has entered
     * \param screenBounds The bounds of the screen
     */
    void advance(const BoundingBox& screenBounds);


 private:
    // The scene's sprites
    Sprite sprites_[SCENE_NUM_SPRITES];
};

#endif  // SCENE_HPP_INCLUDED
