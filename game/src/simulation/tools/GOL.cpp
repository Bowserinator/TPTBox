#include "../ToolClasses.h"
#include "../ToolDefs.h"
#include "../ElementClasses.h"

static void PerformFunc(PERFORM_FUNC_ARGS);

void Tool::Tool_GOL() {
    Color = 0x00DDFFFF;         // These properties will not be visible!
    Identifier = "GOL";         // These properties will not be visible!
    Name = "GOL";               // These properties will not be visible!
    Description = "GOL tool.";  // These properties will not be visible!

    MenuSection = MenuCategory::HIDDEN;
    Enabled = true;
    Perform = &PerformFunc;
};

static void PerformFunc(PERFORM_FUNC_ARGS) {
    int id = sim.create_part(x, y, z, PT_GOL);
    if (id >= 0) { // miscData = GOL ID
        parts[id].tmp2 = miscData;
        parts[id].life = golRules[miscData - 1].decayTime;
        sim.gol.gol_map[z][y][x] = miscData;
        sim._set_color_data_at(x, y, z, &parts[id]); // Force color update for type
    }
}
