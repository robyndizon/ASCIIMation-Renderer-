/**
 * \file boundingbox.hpp
 * \author CS 70 Provided Code
 *
 * \brief Interface definition for the BoundingBox class
 *
 * \details
 * Represents a rectangular region with position and size.
 * Provides methods for collision detection and boundary checking.
 * Note that x and y can be negative to handle sprites that are partially
 * off-screen.
 *
 */

#ifndef BOUNDINGBOX_HPP_INCLUDED
#define BOUNDINGBOX_HPP_INCLUDED

#include <utility>  // actually unnecessary but CPPLINT (incorrectly) wants it

/**
 * \class BoundingBox
 * \brief Represents a rectangular region with position and size
 *
 * \details
 * Stores the position (x, y) and dimensions (width, height) of a rectangle.
 * Provides methods for collision detection and boundary checking.
 * Note that x and y can be negative to handle sprites that are partially
 * off-screen.
 *
 */
class BoundingBox {
 public:
    int x_ = 0;       // X-coordinate of left edge
    int y_ = 0;       // Y-coordinate of top edge
    int width_ = 0;   // Width of the box
    int height_ = 0;  // Height of the box

    /**
     * \brief Constructs a bounding box with given position and dimensions
     * \param x The x-coordinate of the left edge
     * \param y The y-coordinate of the top edge
     * \param width The width of the box
     * \param height The height of the box
     */
    BoundingBox(int x, int y, int width, int height);

    /**
     * \brief Default constructor creates a zero-sized box at origin
     */
    BoundingBox() = default;

    /**
     * \brief Copy constructor
     * \param other The bounding box to copy from
     */
    BoundingBox(const BoundingBox& other) = default;

    /**
     * \brief Assignment operator
     * \param other The bounding box to assign from
     * \returns A reference to this bounding box
     */
    BoundingBox& operator=(const BoundingBox& other) = default;

    /**
     * \brief Sets the position of the box
     * \param x The new x-coordinate
     * \param y The new y-coordinate
     */
    void setPosition(int x, int y);

    /**
     * \brief Moves the box by the given offset
     * \param dx The change in x-coordinate
     * \param dy The change in y-coordinate
     */
    void move(int dx, int dy);

    /**
     * \brief Checks if this box overlaps with another box
     * \param other The other bounding box
     * \returns True if the boxes overlap, false otherwise
     */
    bool isOverlappingWith(const BoundingBox& other) const;

    /**
     * \brief Checks if this box is completely contained within another box
     * \param container The containing bounding box
     * \returns True if this box is completely inside container, false otherwise
     */
    bool isContainedIn(const BoundingBox& container) const;

    /**
     * \brief Computes the union of this box with another box
     * \param other The other bounding box
     * \returns A new bounding box representing the union
     */
    BoundingBox findUnion(const BoundingBox& other) const;

    /**
     * \brief Computes the intersection of this box with another box
     * \param other The other bounding box
     * \returns A new bounding box representing the intersection
     * \note If there is no intersection, the returned box will have
     *       zero width and height
     */
    BoundingBox findIntersection(const BoundingBox& other) const;

    /**
     * \brief Checks if a point is inside the bounding box
     * \param x The x-coordinate of the point
     * \param y The y-coordinate of the point
     * \returns True if the point (x, y) is inside the box, false otherwise
     */
    bool includesPoint(int x, int y) const;

    /**
     * \brief Wraps the position to stay within bounds (for scrolling effects)
     * \param bounds The boundary box to wrap within
     * \note When the box goes off one edge, it appears on the opposite edge
     */
    void wrapWithin(const BoundingBox& bounds);
};

#endif  // BOUNDINGBOX_HPP_INCLUDED
