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

    MenuSection = MenuCategory::RADIOACTIVE;

    HeatConduct = 255;

    Collision = -1.0f;
    OnCreate = &onCreate;

    DefaultProperties.life = 800;
};

static void onCreate(ON_CREATE_FUNC_ARGS) {
    if (mode != PartCreateMode::BRUSH)
        return;

    constexpr float ISQRT3 = 0.57735027f;
    constexpr float ISQRT2 = 0.70710678f;
    constexpr float DIRECTIONS[][3] = {
        {-ISQRT3, -ISQRT3, -ISQRT3},
        {-ISQRT2, -ISQRT2, 0},
        {-ISQRT3, -ISQRT3, ISQRT3},
        {-ISQRT2, 0, -ISQRT2},
        {-1, 0, 0},
        {-ISQRT2, 0, ISQRT2},
        {-ISQRT3, ISQRT3, -ISQRT3},
        {-ISQRT2, ISQRT2, 0},
        {-ISQRT3, ISQRT3, ISQRT3},
        {0, -ISQRT2, -ISQRT2},
        {0, -1, 0},
        {0, -ISQRT2, ISQRT2},
        {0, 0, -1},
        {0, 0, 1},
        {0, ISQRT2, -ISQRT2},
        {0, 1, 0},
        {0, ISQRT2, ISQRT2},
        {ISQRT3, -ISQRT3, -ISQRT3},
        {ISQRT2, -ISQRT2, 0},
        {ISQRT3, -ISQRT3, ISQRT3},
        {ISQRT2, 0, -ISQRT2},
        {1, 0, 0},
        {ISQRT2, 0, ISQRT2},
        {ISQRT3, ISQRT3, -ISQRT3},
        {ISQRT2, ISQRT2, 0},
        {ISQRT3, ISQRT3, ISQRT3}
    };
    const auto DIR = DIRECTIONS[sim.rng().rand() % 26];
    sim.parts[i].vx = DIR[0];
    sim.parts[i].vy = DIR[1];
    sim.parts[i].vz = DIR[2];
}
