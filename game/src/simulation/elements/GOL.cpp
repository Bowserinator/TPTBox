#include "../ElementClasses.h"

#include <iostream>

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_GOL() {
    State = ElementState::TYPE_SOLID;
    Color = 0xFF0000AA;
    Name = "GOL";

    Weight = 100;

    DefaultProperties.tmp1 = 1;

    Graphics = &graphics;
};

static int graphics(GRAPHICS_FUNC_ARGS) {
    // flags[GraphicsFlagsIdx::GLOW] = true;
    // flags[GraphicsFlagsIdx::REFRACT] = true;
    color = 0xFF0000FF | ((sim.frame_count % 256) << 8);

    return 0;
}
