// Defines the PT_XXXX values
#ifndef ELEMENT_CLASSES_H
#define ELEMENT_CLASSES_H

#include <array>

#include "ElementDefs.h"
#include "Element.h"

#define ELEMENT_NUMBERS_ENUMERATE
#include "ElementNumbers.h"
#undef ELEMENT_NUMBERS_ENUMERATE

std::array<Element, PT_NUM> const &GetElements();

#endif