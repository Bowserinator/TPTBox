#include "../ElementClasses.h"

void Element::Element_WATR() {
    Name = "WATR";
    Description = "Water.";
    State = ElementState::TYPE_LIQUID;
    Color = 0x2030D0FF;
    Weight = 10;

    Advection = 0.2f;

    Loss = 0.96f;
    Gravity = 0.3f;
    Diffusion = 5.0f;

    HeatConduct = 15;

    HighTemperature = R_ZERO_C + 100.0f;
    HighTemperatureTransition = PT_GAS;
};
