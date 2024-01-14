#include "../ElementClasses.h"

void Element::Element_WATR() {
    Name = "WATR";
    State = ElementState::TYPE_LIQUID;
    Color = 0x0000FFFF;
    Weight = 10;

    Loss = 0.96f;
    Gravity = 0.3f;
    Diffusion = 5.0f;
};
