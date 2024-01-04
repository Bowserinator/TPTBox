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

	/*float Advection;         // How much particle is accelerated by moving air, generally -1 to 1
	float AirDrag;           // How much air particle produces in direction of travel
	float AirLoss;           // How much moving air is slowed down by the particle, 1 = no effect, 0 = instantly stops
	float Loss;              // Velocity multiplier per frame
	float Collision;         // Velocity multiplier upon collision
	float Gravity;           // How much particle is affected by gravity
	float NewtonianGravity;  // How much particle is affected by newtonian gravity
	float Diffusion;         // How much particle wiggles
	float HotAir;            // How much particle increases pressure by

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