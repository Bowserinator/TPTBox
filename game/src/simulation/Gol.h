#ifndef SIMULATION_GOL_H
#define SIMULATION_GOL_H

#include "stdint.h"
#include "SimulationDef.h"
#include "../util/types/color.h"
#include "../util/types/persistent_buffer.h"

#include <array>
#include <atomic>

class GOLRule {
public:
    enum class Neighborhood { MOORE = 0, NEUMANN = 1 }; // Moore = 26 3x3x3 cube, Neumann = 6 directly adjacent

    // Generate a gol rule
    // @param survive     - Bit string encoding neighborcounts to survive
    // @param birth        - Bit string encoding neighborcounts to birth
    // @param color        - Color of GOL
    // @param neighborhood - MOORE = 26 neighbors, NEUMANN = 6 direct neighbors
    // @param decayTime    - States to decay before dying, 1 = default ALIVE/DEAD state configuration
    GOLRule(uint32_t survive, uint32_t birth, RGBA color, Neighborhood neighborhood = Neighborhood::MOORE, unsigned int decayTime = 1):
        survive(survive), birth(birth), neighborhood(neighborhood), decayTime(decayTime), color(color) {}

    const uint32_t survive, birth;
    const Neighborhood neighborhood;
    const unsigned int decayTime;
    const RGBA color;
};

extern const std::size_t GOL_RULE_COUNT;
extern const GOLRule golRules[];

class SimulationGol {
public:
    ~SimulationGol();

    uint8_t gol_map[ZRES][YRES][XRES]; // Map of GOL Ids (max 255)
    std::atomic<unsigned int> golCount = 0;
    std::array<bool, ZRES> zsliceHasGol;

    void init();
    void reset();
    void dispatch();
    void wait_and_get();
private:
    unsigned int ssboRules;
    util::PersistentBuffer<2> ssbosData;

    unsigned int golShader;
    unsigned int golProgram;
};

#endif