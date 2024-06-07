#include "Tool.h"

Tool::Tool() {
    Color = 0xFFFFFFFF;
    Identifier = "TOOL";
    Name = "TOOL";
    Description = "Tool not defined";
    MenuSection = MenuCategory::TOOLS;
    Enabled = true;

    Perform = nullptr;
}
