#include "../ElementClasses.h"

void Element::Element_WATR() {
    Name = "WATR";
    Identifier = "WATR";
    Description = "Water.";
    State = ElementState::TYPE_LIQUID;
    Color = 0x2030D0FF;
    Weight = 10;

    MenuSection = MenuCategory::LIQUIDS;

    Advection = 0.2f;

    Loss = 0.96f;
    Gravity = 0.3f;
    Diffusion = 4.0f;

    HeatConduct = 150;

    LowTemperature = 0.0_C;
    LowTemperatureTransition = PT_ICE;
    HighTemperature = 100.0_C;
    HighTemperatureTransition = PT_WTRV;
};
