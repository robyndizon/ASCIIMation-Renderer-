#
# Makefile for CS 70 Asciimation Assignment
#

## Makefile variables
#
# Using variables simplifies changing compiler options and adding new
# executables.
#
#  - CXX: the C++ compiler to use
#  - CPPFLAGS: flags to pass to the C preprocessor
#  - CXXFLAGS: flags to pass to the compiler
#  - LDFLAGS: flags to pass to the linker
#  - TARGETS: names of all the targets in the executables section below

CXX = clang++
CPPFLAGS =
CXXFLAGS = -g -std=c++20 -pedantic -Wall -Wextra
LDFLAGS =
TARGETS = boundingbox-test display-test sprite-test our-movie
#         ^---- TODO: add more executables here

## Phony Targets (These are not actually files to build)

all: $(TARGETS)

clean: 
	rm -f *.o $(TARGETS)
# ^--- important, that's a tab, NOT spaces! true for all build commands

# Magic variables explained:
#   $@ is the target (left side of the colon)
#   $< is the first prerequisite (right side of the colon)
#   $^ is all of the prerequisites (right side of the colon)

## Targets to build object files

boundingbox.o: boundingbox.cpp boundingbox.hpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $<

boundingbox-test.o: boundingbox-test.cpp boundingbox.hpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $<

display-test.o: display-test.cpp display.hpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $<

display.o: display.cpp display.hpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $<

sprite.o: sprite.cpp sprite.hpp boundingbox.hpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $<

sprite-test.o: sprite-test.cpp sprite.hpp boundingbox.hpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $<

asciimation.o: asciimation.cpp asciimation.hpp display.hpp sprite.hpp boundingbox.hpp numsprites.hpp scene.hpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $<

our-movie.o: our-movie.cpp asciimation.hpp display.hpp sprite.hpp boundingbox.hpp numsprites.hpp scene.hpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $<

scene.o: scene.cpp scene.hpp sprite.hpp boundingbox.hpp numsprites.hpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $<

## Targets to build executables
#
# Note: 
#   - boundingbox-test needs to be linked with the testinglogger library
#   - sprite-test needs to be linked with the testinglogger library
#   - display-test needs to be linked with the ncurses library
#   - our-movie needs to be linked with the ncurses library
#
# Note that we don't put the libraries in LDFLAGS because not all
# executables need the same libraries -- if everything needed the same
# libraries, we'd list them in LDFLAGS instead.

boundingbox-test: boundingbox-test.o boundingbox.o
	$(CXX) $(LDFLAGS) -o $@ $^ -ltestinglogger

display-test: display-test.o display.o
	$(CXX) $(LDFLAGS) -o $@ $^ -lncurses

sprite-test: sprite-test.o sprite.o boundingbox.o
	$(CXX) $(LDFLAGS) -o $@ $^ -ltestinglogger

our-movie: our-movie.o asciimation.o sprite.o display.o boundingbox.o scene.o
	$(CXX) $(LDFLAGS) -o $@ $^ -lncurses

