// Defines the PT_XXXX values
#ifndef SIMULATION_ELEMENTCLASSES_H_
#define SIMULATION_ELEMENTCLASSES_H_

#include <array>

#include "ElementDefs.h"
#include "Element.h"

#define ELEMENT_NUMBERS_ENUMERATE
#define PT_NONE 0
#include "ElementNumbers.h"
#undef ELEMENT_NUMBERS_ENUMERATE

std::array<Element, PT_NUM> const &GetElements();

#endif // SIMULATION_ELEMENTCLASSES_H_
