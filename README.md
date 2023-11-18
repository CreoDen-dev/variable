# Variable - A Weakly Typed Variable Library for C++

Variable is a single header library for C++ that provides a Var class which behaves like a weakly typed variable in local scopes. It allows you to store and manipulate different data types without explicitly specifying the type.

## Features
- Single header library
- Supports different operators
- Allows calling member functions from it

## Installation
Simply include the ```variable.hpp``` header file in your C++ project.

## Usage
```c++
#include "variable.hpp"

using variable::Var;

int main() {
    // Creating Var objects with different data types
    Var a = 10;
    Var b = "Hello";
    
    // Using different operators with Var objects
    Var sum = a + 5;
    Var concat = b + " World";
    
    // Calling member functions from Var objects
    Var str = "example";
    Var length = str->size();
    
    return 0;
}
```

For more examples see tests.
