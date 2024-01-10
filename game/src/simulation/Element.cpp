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

	float Advection = 0.0f;
	/*float AirDrag;           // How much air particle produces in direction of travel
	float AirLoss;           // How much moving air is slowed down by the particle, 1 = no effect, 0 = instantly stops
	float Loss;              // Velocity multiplier per frame*/
	float Collision = 0.0f;
	/*float Gravity;           // How much particle is affected by gravity
	float NewtonianGravity;  // How much particle is affected by newtonian gravity*/
	float Diffusion = UNSET_PROPERTY;
	/* float HotAir;            // How much particle increases pressure by

	int Hardness;            // How much its affected by ACID, 0 = no effect, higher = more effect

	int Weight;
	unsigned char HeatConduct;
	unsigned int LatentHeat;

	float LowPressure;
	int LowPressureTransition;
	float HighPressure;
	int HighPressureTransition;
	float LowTemperature;
	int LowTemperatureTransition;
	float HighTemperature;
	int HighTemperatureTransition;*/

    Update = nullptr;
}