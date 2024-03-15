#include "../ElementClasses.h"

void Element::Element_GLAS() {
    State = ElementState::TYPE_SOLID;
    Color = 0xFFFFFF55;
    Name = "GLAS";
    Identifier = "GLAS";
    Description = "Element not defined";

	GraphicsFlags = GraphicsFlags::REFRACT | GraphicsFlags::NO_LIGHTING;
    
	HeatConduct = 160;

	LowTemperature = 0.0f;
	LowTemperatureTransition = Transition::NONE;
	HighTemperature = R_ZERO_C + 1600.0f;
	HighTemperatureTransition = Transition::NONE;

    Weight = 100;
};
