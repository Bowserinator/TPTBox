#include "../ElementClasses.h"

void Element::Element_DUST() {
    Name = "DUST";
    State = ElementState::TYPE_POWDER;
    Color = 0xFFE0A0FF;

    Weight = 20;
    Advection = 0.1f;
    Diffusion = 1.0f;

    Gravity = 0.1f;
    Loss = 1.0f;

    HeatConduct = 10;
    DefaultProperties.temp = 500.0f;
};
