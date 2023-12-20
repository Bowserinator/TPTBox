#ifndef ELEMENT_H
#define ELEMENT_H

#include <string>

using ElementState = unsigned int;

constexpr ElementState TYPE_SOLID = 0x1;
constexpr ElementState TYPE_POWDER = 0x2;
constexpr ElementState TYPE_LIQUID = 0x4;
constexpr ElementState TYPE_GAS = 0x8;
constexpr ElementState TYPE_ENERGY = 0x10;

class Element {
    std::string name; // TODO

    unsigned int properties;
    ElementState state;

    std::string color; // TODO 
};

#endif