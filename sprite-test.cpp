/**
 * \file sprite-test.cpp
 * \author CS 70 Provided Code
 *
 * \brief Tests the Sprite class
 *
 * \details
 * This file contains unit tests for the Sprite class. This program
 * needs to be linked with the CS 70 testing library to compile, i.e.,
 *
 *     -ltestinglogger
 */

#include "sprite.hpp"

#include <sstream>
#include <string>
#include <cs70/testinglogger.hpp>

void test_constructor() {
    TestingLogger log("Sprite::Sprite");

    // Valid sprite data
    std::istringstream validData("3 2\nabcdef\n");
    Sprite sprite1{validData, 5, 10};
    BoundingBox bounds = sprite1.getBoundingBox();
    affirm_expected(bounds.width_, 3);
    affirm_expected(bounds.height_, 2);
    affirm_expected(bounds.x_, 5);
    affirm_expected(bounds.y_, 10);
    affirm_expected(sprite1.getCharAt(0, 0), 'a');
    affirm_expected(sprite1.getCharAt(1, 0), 'b');
    affirm_expected(sprite1.getCharAt(2, 0), 'c');
    affirm_expected(sprite1.getCharAt(0, 1), 'd');
    affirm_expected(sprite1.getCharAt(1, 1), 'e');
    affirm_expected(sprite1.getCharAt(2, 1), 'f');

    // Invalid sprite data (negative dimensions)
    std::istringstream invalidData1("-3 2\nabc\ndef\n");
    std::string exceptionMessage;
    try {
        Sprite sprite2{invalidData1, 0, 0};
        exceptionMessage = "No exception thrown!";
    } catch (const std::runtime_error&) {
        exceptionMessage = "Correct exception thrown";
    } catch (...) {
        exceptionMessage = "Wrong type of exception thrown";
    }
    affirm_expected(exceptionMessage, "Correct exception thrown");

    // Invalid sprite data (too large)
    std::istringstream invalidData2("100 100\n");
    try {
        Sprite sprite3{invalidData2, 0, 0};
        exceptionMessage = "No exception thrown!";
    } catch (const std::runtime_error&) {
        exceptionMessage = "Correct exception thrown";
    } catch (...) {
        exceptionMessage = "Wrong type of exception thrown";
    }
    affirm_expected(exceptionMessage, "Correct exception thrown");

    // Invalid filename
    try {
        Sprite sprite4{"/non/existent/file.txt", 0, 0};
        exceptionMessage = "No exception thrown!";
    } catch (const std::runtime_error&) {
        exceptionMessage = "Correct exception thrown";
    } catch (...) {
        exceptionMessage = "Wrong type of exception thrown";
    }

    log.summarize();
}

int main() {
    TestingLogger log("All tests");

    test_constructor();

    return log.summarize(true) ? 0 : 1;
}
