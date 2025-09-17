#include "simulation/ToolClasses.h"
#include "simulation/ToolDefs.h"

static void PerformFunc(PERFORM_FUNC_ARGS);

void Tool::Tool_AIR() {
    Color = 0xFFFFFFFF;
    Identifier = "AIR";
    Name = "AIR";
    Description = "Create positive pressure.";

    MenuSection = MenuCategory::TOOLS;
    Enabled = true;

    Perform = &PerformFunc;
};

static void PerformFunc(PERFORM_FUNC_ARGS) {
    sim.air.out_of_band_air_updates.emplace_back(x, y, z, brushStrength);
}
