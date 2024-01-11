// TODO

#include "Element.h"

Element::Element() {
    Properties = 0;
    State = ElementState::TYPE_SOLID;

    Color = 0xFFFFFFFF;
    Identifier = "NONE";
    Name = "NONE";
    Description = "Element not defined";

    MenuVisible = false;
    MenuSection = 0; // TODO;
    Enabled = false;

	Advection = 0.0f;
	/*AirDrag;           // How much air particle produces in direction of travel
	AirLoss;           // How much moving air is slowed down by the particle, 1 = no effect, 0 = instantly stops*/
	Loss = 1.0f;
	Collision = 0.0f;
	/*Gravity;           // How much particle is affected by gravity
	NewtonianGravity;  // How much particle is affected by newtonian gravity*/
	Diffusion = UNSET_PROPERTY;
	/* HotAir;            // How much particle increases pressure by

	Hardness;            // How much its affected by ACID, 0 = no effect, higher = more effect

	Weight;
	HeatConduct;
	LatentHeat;

	LowPressure;
	LowPressureTransition;
	HighPressure;
	HighPressureTransition;
	LowTemperature;
	LowTemperatureTransition;
	HighTemperature;
	HighTemperatureTransition;*/

    Update = nullptr;
}