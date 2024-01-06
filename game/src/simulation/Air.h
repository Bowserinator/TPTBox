#ifndef AIR_H
#define AIR_H

#include "SimulationDef.h"

class Simulation;

constexpr unsigned int AIR_CELL_SIZE = 4;
static_assert(XRES % AIR_CELL_SIZE == 0, "XRES must be divisible by AIR_CELL_SIZE");
static_assert(YRES % AIR_CELL_SIZE == 0, "YRES must be divisible by AIR_CELL_SIZE");
static_assert(ZRES % AIR_CELL_SIZE == 0, "ZRES must be divisible by AIR_CELL_SIZE");

constexpr unsigned int AIR_XRES = XRES / AIR_CELL_SIZE;
constexpr unsigned int AIR_YRES = YRES / AIR_CELL_SIZE;
constexpr unsigned int AIR_ZRES = ZRES / AIR_CELL_SIZE;

constexpr unsigned int PRESSURE_IDX = 0;
constexpr unsigned int VX_IDX = 1;
constexpr unsigned int VY_IDX = 2;
constexpr unsigned int VZ_IDX = 3;


struct AirCell {
    float data[4];
};

class Air {
public:
    AirCell cells[AIR_ZRES][AIR_YRES][AIR_XRES];
    AirCell out_cells[AIR_ZRES][AIR_YRES][AIR_XRES];

    void clear();
    void update();

    Simulation & sim;
    Air(Simulation & sim);

private:
    void setEdgesAndWalls();
    void setPressureFromVelocity();
    void setVelocityFromPressure();
    void diffusion();
    void advection();
};


#endif