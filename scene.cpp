/**
 * \file scene.cpp
 * \author Leah and Robyn
 *
 * \brief Implements the Scene class
 *
 */


#include "sprite.hpp"
#include "scene.hpp"

Scene::Scene()
    : sprites_{
        {"mystery1.tsprite", 30, 5},
        {"leah.tsprite", 10, 10},
        {"monkey.tsprite", 50, 20}
    } {
    sprites_[0].setVelocity(1, 0);  // Just horizontal scrolling
    sprites_[1].setVelocity(0, 1);  // Vertical scrolling
    sprites_[2].setVelocity(-1, -1);  // Diagonal monkey
}

const Sprite& Scene::getSprite(int i) const {
    return sprites_[i];
}

int Scene::getNumSprites() const {
    return SCENE_NUM_SPRITES;
}

void Scene::advance(const BoundingBox& screenBounds) {
    for (size_t i = 0; i < SCENE_NUM_SPRITES; ++i) {
        sprites_[i].move(screenBounds);
    }
}
