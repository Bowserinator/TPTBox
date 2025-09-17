#include "simulation/ToolClasses.h"
#include "simulation/ToolDefs.h"

static void PerformFunc(PERFORM_FUNC_ARGS);

void Tool::Tool_HEAT() {
    Color = 0xFFE000FF;
    Identifier = "HEAT";
    Name = "HEAT";
    Description = "Heats the targeted element.";

    MenuSection = MenuCategory::TOOLS;
    Enabled = true;

    Perform = &PerformFunc;
};

static void PerformFunc(PERFORM_FUNC_ARGS) {
    if (parts[i].type)
        sim.heat.heat_updates.push_back(PartHeatDelta {
            .id = i,
            .newTemp = sim.p_temp[i] + brushStrength
        });
}
