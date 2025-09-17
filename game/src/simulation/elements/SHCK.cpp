#include "simulation/Air.h"
#include "simulation/ElementClasses.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_SHCK() {
    State = ElementState::TYPE_POWDER;

    Color         = 0x5CFFB6FF;
    GraphicsFlags = 0x0;
    Identifier    = "SHCK";
    Name          = "SHCK";
    Description   = "Shockwave. Makes a pressure shockwave.";

    MenuVisible = false;
    MenuSection = MenuCategory::FORCE;
    Enabled     = true;
    Causality   = 0;

    Advection = 0.5f;
    AirDrag   = -0.1f;
    /*
    AirLoss;           // How much moving air is slowed down by the particle, 1 = no effect, 0 = instantly stops*/
    Loss      = 1.0f;
    Collision = 0.0f;
    Gravity   = 0.1f;
    /*NewtonianGravity;  // How much particle is affected by newtonian gravity*/
    Diffusion = 1.0f;

    Weight    = 20;
    Flammable = 200;

    HeatConduct = 255;

    HighTemperature           = 0.0f;
    HighTemperatureTransition = Transition::NONE;

    Update = &update;
};

static int update(UPDATE_FUNC_ARGS) {
    int r;
    for (int dz = -1; dz <= 1; dz++)
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++) {
                if (!dx && !dy && !dz) continue;
                r = sim.pmap[z + dz][y + dy][x + dx];
                if (r && TYP(r) != PT_SHCK) {
                    sim.air.add_pv(x, y, z, -15);
                    sim.kill_part(i);
                    return -1;
                }
            }
    return 0;
}
