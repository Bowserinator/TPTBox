#include "../ElementClasses.h"

void Element::Element_ICE() {
    State = ElementState::TYPE_SOLID;
    Color = 0xA0C0FFFF;
    Name = "ICE";
    Identifier = "ICE";
    Description = "Ice. TODO";

	GraphicsFlags = GraphicsFlags::REFRACT | GraphicsFlags::NO_LIGHTING;
    
	HeatConduct = 160;
    Loss = 0.0f;

	LowTemperature = 0.0f;
	LowTemperatureTransition = Transition::NONE;
	HighTemperature = 0.0_C;
	HighTemperatureTransition = PT_WATR;

    DefaultProperties.temp = -4.0 + R_ZERO_C;

    Weight = 100;
};
