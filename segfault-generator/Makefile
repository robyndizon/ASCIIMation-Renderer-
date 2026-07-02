# The target "generateSegfault.o" has two dependencies: "generateSegfault.cpp" and "helloSayer.hpp"
# Make will confirm that the dependenies are up to date and then it will run
# the command on the following line to generate generateSegault.o.

generateSegfault.o: generateSegfault.cpp helloSayer.hpp
	clang++ -c -o generateSegfault.o -g -std=c++17 -pedantic -Wall -Wextra generateSegfault.cpp

# The target "helloSayer.o" has two dependencies "helloSayer.cpp" and
# "helloSayer.hpp". Make will make sure that the command runs if either of these
# files have changed since the last time Make had to make helloSayer.o
# (Note the -c in the command, this means that clang will not link, but will
# just comile into an object (.o) file.  We are naming that output file
# helloSayer.o

helloSayer.o: helloSayer.cpp helloSayer.hpp
	clang++ -c -o helloSayer.o -g -std=c++17 -pedantic -Wall -Wextra helloSayer.cpp

# The target "generateSegfault" has two dependencies "generateSegfault.o" and
# "helloSayer.o"  Make will make sure these two dependencies are up to date and
# then it will run the commend on the following line, to compile an executable
# and give it the name "generateSegfault"

generateSegfault: generateSegfault.o helloSayer.o
	clang++  -o generateSegfault generateSegfault.o helloSayer.o

# This is a "phony" target.  When we tell Make to make clean, it has no
# dependencies and it will just run the command below, which removes all object
# files and the executable (if they exist).

clean:
	rm -rf *.o generateSegfault
