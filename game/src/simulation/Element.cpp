// TODO

#include "Element.h"
#include "Particle.h"
#include "SimulationDef.h"

Element::Element() {
    Properties = 0;
    State = ElementState::TYPE_SOLID;

    Color = 0xFFFFFFFF;
	GraphicsFlags = 0x0;
    Identifier = "NONE";
    Name = "NONE";
    Description = "Element not defined";

    MenuVisible = false;
    MenuSection = 0; // TODO;
    Enabled = false;
	Causality = 0;

	Advection = 0.0f;
	/*AirDrag;           // How much air particle produces in direction of travel
	AirLoss;           // How much moving air is slowed down by the particle, 1 = no effect, 0 = instantly stops*/
	Loss = 1.0f;
	Collision = 0.0f;
	Gravity = 0.0f;
	/*NewtonianGravity;  // How much particle is affected by newtonian gravity*/
	Diffusion = 0.0f;
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

	DefaultProperties.ctype = 0;
	DefaultProperties.life = 0;
	DefaultProperties.temp = R_TEMP;
	DefaultProperties.tmp1 = 0;
	DefaultProperties.tmp2 = 0;
	DefaultProperties.dcolor = RGBA{0, 0, 0, 0};

    Update = nullptr;
	Graphics = nullptr;
}