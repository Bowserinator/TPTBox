#ifndef SIMULATION_GOL_H
#define SIMULATION_GOL_H

#include "stdint.h"
#include "SimulationDef.h"
#include <array>

class SimulationGol {
public:
    ~SimulationGol();

    // We use 1 byte IDs to pass to compute shader = max 255 (rule 0 = empty space, ignored)
    // static const uint32_t GOL_RULES[256]; // TODO struct actually need 3 uint32ts
    uint8_t gol_map[ZRES][YRES][XRES]; // TODO: can be 8 bits but shader needs bit logic

    unsigned int golCount = 0;
    std::array<bool, ZRES> zsliceHasGol;

    void init();
    void dispatch();
    void wait_and_get();
private:
    unsigned int ssboIn, ssboOut;
    unsigned int golShader;
    unsigned int golProgram;
};

#endif