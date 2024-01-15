#include "../ElementClasses.h"

void Element::Element_GAS() {
    Name = "GAS";
    State = ElementState::TYPE_GAS;
    Color = 0x00FF00AA;

    Gravity = 0.005f;
    Diffusion = 1.0f;
};
