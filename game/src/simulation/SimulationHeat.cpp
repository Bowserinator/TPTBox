#include "SimulationHeat.h"
#include <cstring>
#include <glad.h>
#include <iostream>

SimulationHeat::~SimulationHeat() {
    rlUnloadShaderBuffer(ssbo_in);
    rlUnloadShaderBuffer(ssbo_out);
    rlUnloadShaderProgram(golLogicProgram);
}

void SimulationHeat::init() {
    // Game of Life logic compute shader
    out = new float[XRES * YRES * ZRES];

    char *golLogicCode = LoadFileText("resources/shaders/heat.comp");
    golLogicShader = rlCompileShader(golLogicCode, RL_COMPUTE_SHADER);
    golLogicProgram = rlLoadComputeShaderProgram(golLogicShader);
    UnloadFileText(golLogicCode);

    ssbo_in  = rlLoadShaderBuffer(XRES * YRES * ZRES * sizeof(float), NULL, RL_DYNAMIC_COPY);
    ssbo_out = rlLoadShaderBuffer(XRES * YRES * ZRES * sizeof(float), NULL, RL_DYNAMIC_COPY);
}

void SimulationHeat::dispatch() {
    rlEnableShader(golLogicProgram);
    rlBindShaderBuffer(ssbo_in, 1);
    rlBindShaderBuffer(ssbo_out, 1);
    rlComputeShaderDispatch(XRES / 16, YRES / 16, ZRES);
    rlDisableShader();

}

void SimulationHeat::wait_and_get() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    rlReadShaderBuffer(ssbo_in, out, XRES * YRES * ZRES * sizeof(float), 0);
}
