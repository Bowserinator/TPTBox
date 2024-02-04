#include "../ElementClasses.h"

void Element::Element_WATR() {
    Name = "WATR";
    State = ElementState::TYPE_LIQUID;
    Color = 0x2030D0EE;
    Weight = 10;

    Advection = 0.2f;

    Loss = 0.96f;
    Gravity = 0.3f;
    Diffusion = 5.0f;
};
