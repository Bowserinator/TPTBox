#include "../ElementClasses.h"

#include <iostream>

static int graphics(GRAPHICS_FUNC_ARGS);
static void onChangeType(ON_CHANGE_TYPE_FUNC_ARGS);

void Element::Element_GOL() {
    State = ElementState::TYPE_SOLID;
    Color = 0xFF0000AA;
    Name = "GOL";

    MenuSection = MenuCategory::LIFE;

    Weight = 100;
    Loss = 0.0f;

    DefaultProperties.tmp1 = 1;

    Graphics = &graphics;
    OnChangeType = &onChangeType;
};

static int graphics(GRAPHICS_FUNC_ARGS) {
    // flags[GraphicsFlagsIdx::GLOW] = true;
    // flags[GraphicsFlagsIdx::REFRACT] = true;
    color = 0xFF0000FF | ((sim.frame_count % 256) << 8);

    return 0;
}

static void onChangeType(ON_CHANGE_TYPE_FUNC_ARGS) {
    if (newType == PT_GOL) {
        sim.gol.gol_map[z][y][x] = 2;
        sim.gol.golCount++;
        sim.gol.zsliceHasGol[z] = true;
    } else {
        sim.gol.gol_map[z][y][x] = 0;
        sim.gol.golCount--;
    }
}
