#include "../ToolClasses.h"
#include "../ToolDefs.h"

static void PerformFunc(PERFORM_FUNC_ARGS);

void Tool::Tool_HEAT() {
    Color = 0xFFE000FF;
    Identifier = "HEAT";
    Name = "HEAT";
    Description = "Heat tool lol";

    MenuSection = MenuCategory::TOOLS;
    Enabled = true;

    Perform = &PerformFunc;
};

static void PerformFunc(PERFORM_FUNC_ARGS) {
    parts[i].vy -= 5.0f;
}
