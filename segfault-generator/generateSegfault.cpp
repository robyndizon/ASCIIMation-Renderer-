// This file contains methods that will read past the
// end of an array in order to induce a segfault.

#include "helloSayer.hpp"

#include <iostream>

// Instead of importing all of the std namespace, just import what we need!
using std::cout, std::endl;

int main() {
    HelloSayer helloSayer;
    helloSayer.sayHello();

    static const size_t ARRAY_SIZE = 10;

    int array[ARRAY_SIZE];

    size_t i = 0;
    int sum = 0;
    while (true) {  // intentionally loop forever
        sum += array[i];
        cout << "Successfully read array[" << i << "] when the size is really "
             << ARRAY_SIZE << ", sum is " << sum << endl;
        ++i;
    }

    cout << "The sum is " << sum
         << ", though you'll never actually see this message"
         << endl;

    return 0;
}
