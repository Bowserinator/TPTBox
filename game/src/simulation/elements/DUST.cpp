#include "../ElementClasses.h"

void Element::Element_DUST() {
    State = ElementState::TYPE_POWDER;

    Color = 0xFFE0A0FF;
	GraphicsFlags = 0x0;
    Identifier = "DUST";
    Name = "DUST";
    Description = "Dust. Light, flammable powder";

    MenuVisible = false;
    MenuSection = 0; // TODO;
    Enabled = true;
	Causality = 0;

	Advection = 0.1f;
	/*AirDrag;           // How much air particle produces in direction of travel
	AirLoss;           // How much moving air is slowed down by the particle, 1 = no effect, 0 = instantly stops*/
	Loss = 1.0f;
	Collision = 0.0f;
	Gravity = 0.1f;
	/*NewtonianGravity;  // How much particle is affected by newtonian gravity*/
	Diffusion = 1.0f;

    Weight = 20;

	HeatConduct = 255;

	HighTemperature = 0.0f;
	HighTemperatureTransition = Transition::NONE;
};
