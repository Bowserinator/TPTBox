#include "../ElementClasses.h"

void Element::Element_GLAS() {
    State = ElementState::TYPE_SOLID;
    Color = 0xFFFFFF55;
    Name = "GLAS";
    Identifier = "GLAS";
    Description = "TODO glass";

    MenuSection = MenuCategory::SOLIDS;

	GraphicsFlags = GraphicsFlags::REFRACT | GraphicsFlags::NO_LIGHTING;
    
	HeatConduct = 160;
    Loss = 0.0f;

	LowTemperature = 0.0f;
	LowTemperatureTransition = Transition::NONE;
	HighTemperature = 200.0_C;
	HighTemperatureTransition = PT_LAVA;

    Weight = 100;
};
