#ifndef ELEMENT_DEFS_H
#define ELEMENT_DEFS_H

#include "stdint.h"

constexpr unsigned short PT_NUM = 1024; // TODO should be max possible

enum class ElementState : uint8_t { TYPE_SOLID, TYPE_POWDER, TYPE_LIQUID, TYPE_GAS, TYPE_ENERGY };

#endif