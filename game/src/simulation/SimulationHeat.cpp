#include "SimulationHeat.h"
#include <cstring>
#include <glad.h>
#include <iostream>

SimulationHeat::~SimulationHeat() {
    rlUnloadShaderProgram(heatProgram);
}

void SimulationHeat::init() {
    char * heatCode = LoadFileText("resources/shaders/heat.comp");
    heatShader = rlCompileShader(heatCode, RL_COMPUTE_SHADER);
    heatProgram = rlLoadComputeShaderProgram(heatShader);
    UnloadFileText(heatCode);

    ssbosData = util::PersistentBuffer<6>(GL_SHADER_STORAGE_BUFFER, XRES * YRES * ZRES * sizeof(float), util::PBFlags::WRITE_ALT_READ);
}

void SimulationHeat::dispatch() {
    rlEnableShader(heatProgram);
    rlBindShaderBuffer(ssbosData.getId(0), 0);
    rlBindShaderBuffer(ssbosData.getId(1), 1);
    rlComputeShaderDispatch(XRES / 16, YRES / 16, ZRES);
    rlDisableShader();
}

void SimulationHeat::wait_and_get() {
    
}
