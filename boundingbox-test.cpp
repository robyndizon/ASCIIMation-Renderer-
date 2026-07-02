/**
 * \file boundingbox-test.cpp
 * \author CS 70 Provided Code
 *
 * \brief Tests the BoundingBox class
 *
 * \details
 * This file contains unit tests for the BoundingBox class. This program
 * needs to be linked with the CS 70 testing library to compile, i.e.,
 *
 *     -ltestinglogger
 */

#include "boundingbox.hpp"
#include <cs70/testinglogger.hpp>

void test_isOverlappingWith() {
    TestingLogger log("BoundingBox::isOverlappingWith");

    BoundingBox box1{0, 0, 10, 10};
    BoundingBox box2{5, 5, 10, 10};
    BoundingBox box3{9, 9, 5, 5};
    BoundingBox box4{10, 10, 5, 5};
    BoundingBox box5{-5, -5, 3, 3};
    BoundingBox box6{-1, -1, 3, 3};

    affirm(box1.isOverlappingWith(box2));   // Partial overlap
    affirm(box2.isOverlappingWith(box1));   // Symmetric
    affirm(box1.isOverlappingWith(box3));   // Touching at corner
    affirm(!box1.isOverlappingWith(box4));  // No overlap
    affirm(!box1.isOverlappingWith(box5));  // No overlap
    affirm(box1.isOverlappingWith(box6));   // Partial overlap

    log.summarize();
}

void test_isContainedIn() {
    TestingLogger log("BoundingBox::isContainedIn");

    BoundingBox container{0, 0, 10, 10};
    BoundingBox inside{2, 2, 5, 5};
    BoundingBox touchingEdge{0, 0, 10, 10};
    BoundingBox outside{11, 11, 2, 2};
    BoundingBox partiallyOutside{8, 8, 5, 5};

    affirm(inside.isContainedIn(container));
    affirm(touchingEdge.isContainedIn(container));
    affirm(!outside.isContainedIn(container));
    affirm(!partiallyOutside.isContainedIn(container));

    log.summarize();
}

void test_findIntersection() {
    TestingLogger log("BoundingBox::findIntersection");

    BoundingBox box1{0, 0, 10, 10};
    BoundingBox box2{5, 5, 10, 10};
    BoundingBox box3{9, 9, 5, 5};
    BoundingBox box4{10, 10, 5, 5};
    BoundingBox box5{-5, -5, 3, 3};
    BoundingBox box6{-1, -1, 3, 3};

    BoundingBox inter1 = box1.findIntersection(box2);
    affirm_expected(inter1.x_, 5);
    affirm_expected(inter1.y_, 5);
    affirm_expected(inter1.width_, 5);
    affirm_expected(inter1.height_, 5);

    BoundingBox inter2 = box1.findIntersection(box3);
    affirm_expected(inter2.x_, 9);
    affirm_expected(inter2.y_, 9);
    affirm_expected(inter2.width_, 1);
    affirm_expected(inter2.height_, 1);

    BoundingBox inter3 = box1.findIntersection(box4);
    affirm_expected(inter3.width_, 0);
    affirm_expected(inter3.height_, 0);

    BoundingBox inter4 = box1.findIntersection(box5);
    affirm_expected(inter4.x_, 0);
    affirm_expected(inter4.y_, 0);
    affirm_expected(inter4.width_, 0);
    affirm_expected(inter4.height_, 0);

    BoundingBox inter5 = box1.findIntersection(box6);
    affirm_expected(inter5.x_, 0);
    affirm_expected(inter5.y_, 0);
    affirm_expected(inter5.width_, 2);
    affirm_expected(inter5.height_, 2);

    log.summarize();
}

void test_findUnion() {
    TestingLogger log("BoundingBox::findUnion");

    BoundingBox box1{0, 0, 10, 10};
    BoundingBox box2{5, 5, 10, 10};
    BoundingBox box3{9, 9, 5, 5};
    BoundingBox box4{10, 10, 5, 5};
    BoundingBox box5{-5, -5, 3, 3};
    BoundingBox box6{-1, -1, 3, 3};

    BoundingBox union1 = box1.findUnion(box2);
    affirm_expected(union1.x_, 0);
    affirm_expected(union1.y_, 0);
    affirm_expected(union1.width_, 15);
    affirm_expected(union1.height_, 15);

    BoundingBox union2 = box1.findUnion(box3);
    affirm_expected(union2.x_, 0);
    affirm_expected(union2.y_, 0);
    affirm_expected(union2.width_, 14);
    affirm_expected(union2.height_, 14);

    BoundingBox union3 = box1.findUnion(box4);
    affirm_expected(union3.x_, 0);
    affirm_expected(union3.y_, 0);
    affirm_expected(union3.width_, 15);
    affirm_expected(union3.height_, 15);

    BoundingBox union4 = box1.findUnion(box5);
    affirm_expected(union4.x_, -5);
    affirm_expected(union4.y_, -5);
    affirm_expected(union4.width_, 15);
    affirm_expected(union4.height_, 15);

    BoundingBox union5 = box1.findUnion(box6);
    affirm_expected(union5.x_, -1);
    affirm_expected(union5.y_, -1);
    affirm_expected(union5.width_, 11);
    affirm_expected(union5.height_, 11);

    log.summarize();
}

void test_wrapWithin() {
    TestingLogger log("BoundingBox::wrapWithin");

    BoundingBox bounds{0, 0, 10, 10};
    BoundingBox box1{11, 5, 3, 3};   // Right of bounds
    BoundingBox box2{-4, 5, 3, 3};   // Left of bounds
    BoundingBox box3{5, 11, 3, 3};   // Below bounds
    BoundingBox box4{5, -4, 3, 3};   // Above bounds
    BoundingBox box5{12, 12, 3, 3};  // Bottom-right corner
    BoundingBox box6{-4, -4, 3, 3};  // Top-left corner

    box1.wrapWithin(bounds);
    // affirm(box1.x_ == -3 && box1.y_ == 5);
    affirm_expected(box1.x_, -3);
    affirm_expected(box1.y_, 5);

    box2.wrapWithin(bounds);
    affirm_expected(box2.x_, 10);
    affirm_expected(box2.y_, 5);

    box3.wrapWithin(bounds);
    affirm_expected(box3.x_, 5);
    affirm_expected(box3.y_, -3);

    box4.wrapWithin(bounds);
    affirm_expected(box4.x_, 5);
    affirm_expected(box4.y_, 10);

    box5.wrapWithin(bounds);
    affirm_expected(box5.x_, -3);
    affirm_expected(box5.y_, -3);

    box6.wrapWithin(bounds);
    affirm_expected(box6.x_, 10);
    affirm_expected(box6.y_, 10);

    log.summarize();
}

int main() {
    TestingLogger log("BoundingBox tests");

    test_isOverlappingWith();
    test_isContainedIn();
    test_findIntersection();
    test_findUnion();
    test_wrapWithin();

    if (log.summarize(true)) {
        return 0;  // success
    } else {
        return 1;  // failure
    }
}
