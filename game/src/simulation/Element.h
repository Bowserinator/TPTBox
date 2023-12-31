#ifndef ELEMENT_H
#define ELEMENT_H

#include <string>

#include "ElementDefs.h"


class Element {
public:
    // TODO: nuermic id, menu section, menu visible, enabled
    // gravity and air, diffusion, etc...
    // heat conduction
    // 

    std::string name; // TODO

    unsigned int properties;
    ElementState state;

    std::string color; // TODO 

    int (*Update)();

    // TODO: graphics, create function, create allowed, change type, ctype draw
    // todo: default properties

    Element();

    // Define void Element_NAME(); for each element
    #define ELEMENT_NUMBERS_DECLARE
    #include "ElementNumbers.h"
    #undef ELEMENT_NUMBERS_DECLARE
};

#endif