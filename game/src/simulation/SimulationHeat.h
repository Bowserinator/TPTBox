#ifndef SIMULATION_HEAT_H
#define SIMULATION_HEAT_H

#include "raylib.h"
#include "rlgl.h"
#include "../util/types/persistent_buffer.h"
#include "SimulationDef.h"

class SimulationHeat {
public:
    ~SimulationHeat();

    void init();
    void dispatch();
    void wait_and_get();
private:
    util::PersistentBuffer<6> ssbosData;

    unsigned int heatShader;
    unsigned int heatProgram;
    float * out;
};

#endif