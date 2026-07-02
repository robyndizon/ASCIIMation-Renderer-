/**
 * \file boundingbox.cpp
 * \author CS 70 Provided Code
 *
 * \brief Implements the BoundingBox class
 *
 */

#include "boundingbox.hpp"
#include <iostream>
#include <algorithm>  // for std::min and std::max
BoundingBox::BoundingBox(int x, int y, int width, int height)
    : x_{x}, y_{y}, width_{width}, height_{height} {
    // Nothing else to do
}

void BoundingBox::setPosition(int x, int y) {
    x_ = x;
    y_ = y;
}

void BoundingBox::move(int dx, int dy) {
    x_ += dx;
    y_ += dy;
}

bool BoundingBox::isOverlappingWith(const BoundingBox& other) const {
    // Two boxes overlap if they share any area
    // They don't overlap if one is completely to the left, right, above, or
    // below the other
    return !(x_ >= other.x_ + other.width_ || other.x_ >= x_ + width_
             || y_ >= other.y_ + other.height_ || other.y_ >= y_ + height_);
}

bool BoundingBox::isContainedIn(const BoundingBox& container) const {
    return x_ >= container.x_ && y_ >= container.y_
           && x_ + width_ <= container.x_ + container.width_
           && y_ + height_ <= container.y_ + container.height_;
}

BoundingBox BoundingBox::findUnion(const BoundingBox& other) const {
    int new_x = std::min(x_, other.x_);
    int new_y = std::min(y_, other.y_);
    int new_right = std::max(x_ + width_, other.x_ + other.width_);
    int new_bottom = std::max(y_ + height_, other.y_ + other.height_);
    return BoundingBox(new_x, new_y, new_right - new_x, new_bottom - new_y);
}

BoundingBox BoundingBox::findIntersection(const BoundingBox& other) const {
    int new_x = std::max(x_, other.x_);
    int new_y = std::max(y_, other.y_);
    int new_right = std::min(x_ + width_, other.x_ + other.width_);
    int new_bottom = std::min(y_ + height_, other.y_ + other.height_);
    if (new_x >= new_right || new_y >= new_bottom) {
        // No intersection
        return BoundingBox();
    }
    return BoundingBox(new_x, new_y, new_right - new_x, new_bottom - new_y);
}

bool BoundingBox::includesPoint(int x, int y) const {
    return x >= x_ && x < x_ + width_ && y >= y_ && y < y_ + height_;
}

void BoundingBox::wrapWithin(const BoundingBox& bounds) {
    // Defines the sides of our boxes
    int ourLeft = x_;
    int ourRight = x_ + width_;
    int ourTop = y_;
    int ourBottom = y_ + height_;
    int boundsLeft = bounds.x_;
    int boundsRight = bounds.x_ + bounds.width_;
    int boundsTop = bounds.y_;
    int boundsBottom = bounds.y_ + bounds.height_;

    // check if too far right or left
    if (ourLeft > boundsRight) {
        setPosition(boundsLeft - width_, y_);
    } else if (ourRight < boundsLeft) {
        setPosition(boundsRight, y_);
    }
    // check if too far up or down
    if (ourBottom < boundsTop) {
        setPosition(x_, boundsBottom);
    } else if (ourTop > boundsBottom) {
        setPosition(x_, boundsTop - height_);
    }
}

