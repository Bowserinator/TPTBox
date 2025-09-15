#include "../ElementClasses.h"
#include "../../graphics/gradient.h"

static int FIRE_graphics(GRAPHICS_FUNC_ARGS);
static int update(UPDATE_FUNC_ARGS);
static void onCreate(ON_CREATE_FUNC_ARGS);

constexpr float DEFAULT_FIRE_TEMP = R_TEMP + 300.0f;

void Element::Element_FIRE() {
    Name = "FIRE";
    Identifier = "FIRE";
    Description = "Fire. Burns";
    State = ElementState::TYPE_GAS;
    Color = 0xFF0000CC;
    GraphicsFlags = GraphicsFlags::GLOW | GraphicsFlags::BLUR | GraphicsFlags::NO_LIGHTING;
    Properties = ElementProperties::LIFE_DEC_KILL;

    MenuSection = MenuCategory::EXPLOSIVE;

    Gravity = -0.1f;
    Diffusion = 1.0f;
    Advection = 1.0f;

    HeatConduct = 255;

    Graphics = &FIRE_graphics;
    Update = &update;
    OnCreate = &onCreate;

    DefaultProperties.temp = DEFAULT_FIRE_TEMP;
};

static void onCreate(ON_CREATE_FUNC_ARGS) {
    sim.parts[i].life = sim.rng().between(120, 169);
}

static int FIRE_graphics(GRAPHICS_FUNC_ARGS) {
    color = graphics::gradient_get(graphics::gradients::fire_gradient, part.life / 169.0f);
    flags = GraphicsFlags::GLOW | GraphicsFlags::NO_LIGHTING;
    return 0;
}

static int update(UPDATE_FUNC_ARGS) {
    int r;
    for (int dz = -1; dz <= 1; dz++)
    for (int dy = -1; dy <= 1; dy++)
    for (int dx = -1; dx <= 1; dx++) {
        if (!dx && !dy && !dz) continue;
        r = sim.pmap[z + dz][y + dy][x + dx];
        if (r && TYP(r) != PT_FIRE) {
            const auto &el = GetElements()[TYP(r)];
            if (el.Flammable && sim.rng().chance(el.Flammable, 1024)) {
                sim.parts[ID(r)].life = sim.rng().between(120, 169);
                sim.p_temp[ID(r)] = std::max(sim.p_temp[ID(r)], DEFAULT_FIRE_TEMP + el.Flammable / 2);
                sim.part_change_type(ID(r), PT_FIRE);
                goto end;
            }
        }
    }
end:
    return 0;
}
