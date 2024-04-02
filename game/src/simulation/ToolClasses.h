// Defines the TOOL_XXXX values
#ifndef TOOL_CLASSES_H
#define TOOL_CLASSES_H

#include <array>

#include "Tool.h"

#define TOOL_NUMBERS_ENUMERATE
#define PT_NONE 0
#include "ToolNumbers.h"
#undef TOOL_NUMBERS_ENUMERATE

std::array<Tool, __GLOBAL_TOOL_COUNT + 1> const &GetTools();

#endif