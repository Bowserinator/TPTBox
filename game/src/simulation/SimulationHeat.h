#ifndef SIMULATION_HEAT_H
#define SIMULATION_HEAT_H

#include "raylib.h"
#include "rlgl.h"
#include "../util/types/persistent_buffer.h"
#include "SimulationDef.h"

class Simulation;

class SimulationHeat {
public:
    ~SimulationHeat();

    float heat_map[ZRES][YRES][XRES];
    bool uploadedOnce = false;
    Simulation * sim = nullptr;

    void init();
    void dispatch();
    void wait_and_get();
private:
    util::PersistentBuffer<6> ssbosData;
    unsigned int ssboConstants;

    unsigned int heatShader;
    unsigned int heatProgram;
};

#endif