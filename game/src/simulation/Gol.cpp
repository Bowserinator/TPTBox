#include "Gol.h"
#include "raylib.h"
#include "rlgl.h"
#include "../util/graphics.h"
#include "../util/vector_op.h"

#include <cstring>
#include <glad.h>
#include <iostream>

// Encode neighbors as bits in a bitstring for GOL
// Usage: gRule({1, 2, 3})
// @param list - List of neighbors where rule is valid, at most 26
// @return Bit encoded string, ie {1,2,3} -> 0b1110
constexpr uint32_t gRule(std::initializer_list<unsigned int> list) {
    uint32_t out = 0;
    for (const auto elem : list)
        out |= (1 << elem);
    return out;
}

const std::size_t GOL_RULE_COUNT = 2;

const GOLRule golRules[GOL_RULE_COUNT] = {
    GOLRule(gRule({5,6,7}), gRule({6}), RGBA(0xFF0000FF), GOLRule::Neighborhood::MOORE, 1),
    GOLRule(gRule({0,1,2,3,4,5,6}), gRule({1,3}), RGBA(0xFF0000FF), GOLRule::Neighborhood::NEUMANN, 1),
};


// GOL SIM
// -----------------------------
SimulationGol::~SimulationGol() {
    rlUnloadShaderBuffer(ssboRules);
    rlUnloadShaderProgram(golProgram);
}

void SimulationGol::init() {
    char *golLogicCode = LoadFileText("resources/shaders/gol.comp");
    golShader = rlCompileShader(golLogicCode, RL_COMPUTE_SHADER);
    golProgram = rlLoadComputeShaderProgram(golShader);
    UnloadFileText(golLogicCode);

    struct {
        int32_t SIMRES[4] = { (int)XRES, (int)YRES, (int)ZRES };
        uint32_t data[GOL_RULE_COUNT * 2];
    } golRuleData;

    ssbosData = util::MultibufferSSBO(2, sizeof(gol_map), RL_DYNAMIC_COPY);
    ssboRules = rlLoadShaderBuffer(sizeof(golRuleData), NULL, RL_STATIC_READ);

    for (auto i = 0; i < GOL_RULE_COUNT; i++) {
        golRuleData.data[2*i]   = golRules[i].birth | (static_cast<uint64_t>(golRules[i].neighborhood) << 31);
        golRuleData.data[2*i+1] = golRules[i].survive;
    }

    rlUpdateShaderBuffer(ssboRules, &golRuleData, sizeof(golRuleData), 0);
    memset(gol_map, 0, sizeof(gol_map));
}

void SimulationGol::dispatch() {
    rlUpdateShaderBuffer(ssbosData.get(0), gol_map, sizeof(gol_map), 0);

    rlEnableShader(golProgram);
    rlBindShaderBuffer(ssboRules, 0);
    rlBindShaderBuffer(ssbosData.get(0), 1);
    rlBindShaderBuffer(ssbosData.get(1), 2);
    rlComputeShaderDispatch(XRES / 8 + 1, YRES / 8 + 1, ZRES / 8 + 1);
    rlDisableShader();
}

void SimulationGol::wait_and_get() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    rlReadShaderBuffer(ssbosData.get(1), gol_map, sizeof(gol_map), 0);
    ssbosData.advance_cycle();
}