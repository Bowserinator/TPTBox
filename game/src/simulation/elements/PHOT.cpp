#include "../ElementClasses.h"

void Element::Element_PHOT() {
    Name = "PHOT";
    State = ElementState::TYPE_ENERGY;
    Color = 0xFFFFFFAA;
    GraphicsFlags = GraphicsFlags::GLOW;

    Collision = -1.0f;
};
