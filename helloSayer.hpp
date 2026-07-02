/*
 * A very simple example class
 */

// C++ include guard for header file.
// Its mere presence will prevent linker errors and much sorrow later!
#ifndef HELLOSAYER_HPP_INCLUDED
#define HELLOSAYER_HPP_INCLUDED

// The only thing the HelloSayer class does is... say hello!
class HelloSayer {
 public:
    void sayHello() const;
};

// The next line ends the include guard for this header file:
#endif  // HELLOSAYER_HPP_INCLUDED
