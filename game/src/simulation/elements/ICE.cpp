#include "../ElementClasses.h"

void Element::Element_ICE() {
    State = ElementState::TYPE_SOLID;
    Color = 0xA0C0FFFF;
    Name = "ICE";
    Identifier = "ICE";
    Description = "Ice. TODO";

    MenuSection = MenuCategory::SOLIDS;

	GraphicsFlags = GraphicsFlags::REFRACT | GraphicsFlags::NO_LIGHTING;
    
	HeatConduct = 160;
    Loss = 0.0f;

	HighTemperature = 0.0_C;
	HighTemperatureTransition = Transition::TO_CTYPE;

    DefaultProperties.temp = -4.0 + R_ZERO_C;
    DefaultProperties.ctype = PT_WATR;

    Weight = 100;
};
