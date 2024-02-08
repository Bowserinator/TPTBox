#include "../ElementClasses.h"

void Element::Element_GAS() {
    Name = "GAS";
    State = ElementState::TYPE_GAS;
    Color = 0xE0FF20CC;
    GraphicsFlags = GraphicsFlags::BLUR | GraphicsFlags::NO_LIGHTING;

    Gravity = 0.005f;
    Diffusion = 1.0f;
};
