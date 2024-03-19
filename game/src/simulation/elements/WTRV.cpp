#include "../ElementClasses.h"

void Element::Element_WTRV() {
    Name = "WTRV";
    Identifier = "WTRV";
    Description = "Water Vapor.";
    State = ElementState::TYPE_GAS;
    Color = 0xA0A0FFCC;
    GraphicsFlags = GraphicsFlags::BLUR | GraphicsFlags::NO_LIGHTING;

    Gravity = -0.005f;
    Diffusion = 1.0f;
    HeatConduct = 255;

    LowTemperature = 100.0_C;
    LowTemperatureTransition = PT_WATR;

    DefaultProperties.temp = 101.0_C;
};
