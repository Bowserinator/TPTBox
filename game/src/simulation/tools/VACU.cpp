#include "simulation/ToolClasses.h"
#include "simulation/ToolDefs.h"

static void PerformFunc(PERFORM_FUNC_ARGS);

void Tool::Tool_VACU() {
    Color = 0x333333FF;
    Identifier = "VACU";
    Name = "VACU";
    Description = "Create negative pressure.";

    MenuSection = MenuCategory::TOOLS;
    Enabled = true;

    Perform = &PerformFunc;
};

static void PerformFunc(PERFORM_FUNC_ARGS) {
    sim.air.add_out_of_band_update(x, y, z, -brushStrength * 0.01f);
}
