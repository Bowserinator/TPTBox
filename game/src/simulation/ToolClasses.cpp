#include "ToolClasses.h"

// This is literally stolen from TPT
std::array<Tool, __GLOBAL_TOOL_COUNT + 1> const &GetTools() {
    struct DoOnce {
        std::array<Tool, __GLOBAL_TOOL_COUNT + 1> tools;

        DoOnce() {
#define TOOL_NUMBERS_CALL
#include "ToolNumbers.h"
#undef TOOL_NUMBERS_CALL
        }
    };

    static DoOnce doOnce;
    return doOnce.tools;
}
