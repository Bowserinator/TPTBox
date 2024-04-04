#include "../ElementClasses.h"

static int graphics(GRAPHICS_FUNC_ARGS);
static void onChangeType(ON_CHANGE_TYPE_FUNC_ARGS);

void Element::Element_GOL() {
    State = ElementState::TYPE_SOLID;
    Color = 0xFF0000AA;
    Name = "GOL";

    // Hidden, we create it through tools added in SimUI.cpp
    MenuSection = MenuCategory::HIDDEN;

    Weight = 100;
    Loss = 0.0f;

    // tmp1: Whether dying
    // tmp2: GOL ID
    // life: Frames remaining before death
    DefaultProperties.tmp1 = 0;
    DefaultProperties.tmp2 = 2;
    DefaultProperties.life = 1;

    Graphics = &graphics;
    OnChangeType = &onChangeType;
};

static int graphics(GRAPHICS_FUNC_ARGS) {
    color = golRules[util::clamp(part.tmp2, 1, GOL_RULE_COUNT) - 1].color;
    return 0;
}

static void onChangeType(ON_CHANGE_TYPE_FUNC_ARGS) {
    if (newType == PT_GOL) {
        sim.gol.gol_map[z][y][x] = sim.parts[i].tmp2;
        sim.gol.golCount++;
        sim.gol.zsliceHasGol[z] = true;
    } else {
        sim.gol.gol_map[z][y][x] = 0;
        sim.gol.golCount--;
    }
}
