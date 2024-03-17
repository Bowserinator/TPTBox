#include "../ElementClasses.h"

void Element::Element_LN2() {
    Name = "LN2";
    Identifier = "LN2";
    Description = "Liquid Nitrogen.";
    State = ElementState::TYPE_LIQUID;
    Color = 0x80A0DFFF;
    Weight = 10;

    Advection = 0.2f;

    Loss = 0.96f;
    Gravity = 0.3f;
    Diffusion = 5.0f;

    HeatConduct = 255;

    HighTemperature = -100.0 + R_ZERO_C; // TODO
    HighTemperatureTransition = PT_NONE;

    DefaultProperties.temp = -200.0 + R_ZERO_C;
};
