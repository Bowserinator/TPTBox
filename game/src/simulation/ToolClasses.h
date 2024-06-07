// Defines the TOOL_XXXX values
#ifndef SIMULATION_TOOLCLASSES_H_
#define SIMULATION_TOOLCLASSES_H_

#include <array>

#include "Tool.h"

#define TOOL_NUMBERS_ENUMERATE
#define PT_NONE 0
#include "ToolNumbers.h"
#undef TOOL_NUMBERS_ENUMERATE

std::array<Tool, __GLOBAL_TOOL_COUNT + 1> const &GetTools();

#endif // SIMULATION_TOOLCLASSES_H_
