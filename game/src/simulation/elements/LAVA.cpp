#include "../ElementClasses.h"

void Element::Element_LAVA() {
    Name = "LAVA";
    Identifier = "LAVA";
    Description = "Lava.";
    State = ElementState::TYPE_LIQUID;
    Color = 0xFFBA6BFF;
    Weight = 10;

    MenuSection = MenuCategory::LIQUIDS;

    GraphicsFlags = GraphicsFlags::GLOW | GraphicsFlags::NO_LIGHTING;

    Advection = 0.2f;

    Loss = 0.96f;
    Gravity = 0.3f;
    Diffusion = 0.5f;

    HeatConduct = 150;

    LowTemperature = 200.0_C;
    LowTemperatureTransition = Transition::TO_CTYPE;

    DefaultProperties.temp = 900.0_C;
    DefaultProperties.ctype = PT_DUST;
};
