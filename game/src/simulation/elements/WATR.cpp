#include "../ElementClasses.h"

void Element::Element_WATR() {
    State = ElementState::TYPE_LIQUID;
    Color = 0x0000FFFF;
    Weight = 10;

    Diffusion = UNSET_PROPERTY;
};
