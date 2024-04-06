#ifndef TOOL_H
#define TOOL_H

#include <string>
#include "stdint.h"

#include "ToolDefs.h"
#include "MenuCategories.h"
#include "../graphics/color.h"
#include "../util/types/bitset8.h"

class Tool {
public:
	bool Enabled;            // If enabled = false tool cannot be used
    std::string Identifier;  // ID string, like HEAT
	std::string Name;        // Display name, like HEAT
    std::string Description; // Menu description
    RGBA Color;

	MenuCategory MenuSection;
    
    void (*Perform)(PERFORM_FUNC_ARGS);

    Tool();
	Tool(const Tool&) = delete;
	Tool& operator=(const Tool&) = delete;

    // Define void Tool_NAME(); for each tool
    #define TOOL_NUMBERS_DECLARE
    #include "ToolNumbers.h"
    #undef TOOL_NUMBERS_DECLARE
};

#endif