#include "Gol.h"
#include "raylib.h"
#include "rlgl.h"

#include <cstring>
#include <glad.h>
#include <iostream>

SimulationGol::~SimulationGol() {
    rlUnloadShaderBuffer(ssboIn);
    rlUnloadShaderBuffer(ssboOut);
    rlUnloadShaderProgram(golProgram);
}

void SimulationGol::init() {
    char *golLogicCode = LoadFileText("resources/shaders/gol.comp");
    golShader = rlCompileShader(golLogicCode, RL_COMPUTE_SHADER);
    golProgram = rlLoadComputeShaderProgram(golShader);
    UnloadFileText(golLogicCode);

    memset(gol_map, 0, sizeof(gol_map));
    ssboIn  = rlLoadShaderBuffer(sizeof(gol_map), NULL, RL_DYNAMIC_DRAW);
    ssboOut = rlLoadShaderBuffer(sizeof(gol_map), NULL, RL_DYNAMIC_READ);
}

void SimulationGol::dispatch() {
    rlUpdateShaderBuffer(ssboIn, gol_map, sizeof(gol_map), 0);

    rlEnableShader(golProgram);
    rlBindShaderBuffer(ssboIn, 1);
    rlBindShaderBuffer(ssboOut, 2);
    rlComputeShaderDispatch(XRES / 8 + 1, YRES / 8 + 1, ZRES / 8 + 1);
    rlDisableShader();
}

void SimulationGol::wait_and_get() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    rlReadShaderBuffer(ssboOut, gol_map, sizeof(gol_map), 0);
}