#include "simulation/ToolClasses.h"
#include "simulation/ToolDefs.h"

static void PerformFunc(PERFORM_FUNC_ARGS);

void Tool::Tool_COOL() {
    Color = 0x00DDFFFF;
    Identifier = "COOL";
    Name = "COOL";
    Description = "Cools the targeted element.";

    MenuSection = MenuCategory::TOOLS;
    Enabled = true;

    Perform = &PerformFunc;
};

static void PerformFunc(PERFORM_FUNC_ARGS) {
    if (parts[i].type)
        sim.heat.heat_updates.push_back(PartHeatDelta {
            .id = i,
            .newTemp = sim.p_temp[i] - brushStrength
        });
}
