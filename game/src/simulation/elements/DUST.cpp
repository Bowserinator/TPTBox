#include "simulation/ElementClasses.h"

void Element::Element_DUST() {
    State = ElementState::TYPE_POWDER;

    Color         = 0xFFE0A0FF;
    GraphicsFlags = 0x0;
    Identifier    = "DUST";
    Name          = "DUST";
    Description   = "Dust. Light, flammable powder";

    MenuVisible = false;
    MenuSection = MenuCategory::POWDERS;
    Enabled     = true;
    Causality   = 0;

    Advection = 0.5f;
    AirDrag   = 0.0001f;
    AirLoss   = 1.0f;
    Loss      = 0.96f;
    Collision = 0.0f;
    Gravity   = 0.1f;
    /*NewtonianGravity;  // How much particle is affected by newtonian gravity*/
    Diffusion = 1.0f;

    Weight    = 20;
    Flammable = 200;

    HeatConduct = 255;

    HighTemperature           = 0.0f;
    HighTemperatureTransition = Transition::NONE;
};
