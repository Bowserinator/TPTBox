#include "../ElementClasses.h"

static void onCreate(ON_CREATE_FUNC_ARGS);

void Element::Element_PHOT() {
    Name = "PHOT";
    Identifier = "PHOT";
    Description = "Photons.";
    State = ElementState::TYPE_ENERGY;
    Color = 0xFFFFFFFF;
    GraphicsFlags = GraphicsFlags::GLOW | GraphicsFlags::NO_LIGHTING;
    Properties = ElementProperties::LIFE_DEC_KILL;

    HeatConduct = 255;

    Collision = -1.0f;
    OnCreate = &onCreate;

    DefaultProperties.life = 200;
};

static void onCreate(ON_CREATE_FUNC_ARGS) {
    constexpr float DIRECTIONS[][3] = {
        {-1, -1, -1},
        {-1, -1, 0},
        {-1, -1, 1},
        {-1, 0, -1},
        {-1, 0, 0},
        {-1, 0, 1},
        {-1, 1, -1},
        {-1, 1, 0},
        {-1, 1, 1},
        {0, -1, -1},
        {0, -1, 0},
        {0, -1, 1},
        {0, 0, -1},
        {0, 0, 1},
        {0, 1, -1},
        {0, 1, 0},
        {0, 1, 1},
        {1, -1, -1},
        {1, -1, 0},
        {1, -1, 1},
        {1, 0, -1},
        {1, 0, 0},
        {1, 0, 1},
        {1, 1, -1},
        {1, 1, 0},
        {1, 1, 1}
    };
    const auto DIR = DIRECTIONS[sim.rng().rand() % 26];
    sim.parts[i].vx = DIR[0];
    sim.parts[i].vy = DIR[1];
    sim.parts[i].vz = DIR[2];
}
