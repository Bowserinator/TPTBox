#include "../ElementClasses.h"

void Element::Element_DUST() {
    State = ElementState::TYPE_POWDER;
    Color = 0xFFFF00FF;
    Weight = 20;
    Advection = 0.1f;
};
