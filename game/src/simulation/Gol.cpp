#include "Gol.h"
#include "raylib.h"
#include "rlgl.h"

#include "../util/graphics.h"
#include "../util/vector_op.h"
#include "../util/types/gl_time_query.h"
#include "../render/constants.h"

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
#ifdef EMBED_SHADERS
    #include "../../resources/shaders/generated/gol.comp.h"
    golShader = rlCompileShader(gol_comp_source, RL_COMPUTE_SHADER);
    golProgram = rlLoadComputeShaderProgram(golShader);
#else
    char * gol_comp_source = LoadFileText("resources/shaders/gol.comp");
    golShader = rlCompileShader(gol_comp_source, RL_COMPUTE_SHADER);
    golProgram = rlLoadComputeShaderProgram(golShader);
    UnloadFileText(gol_comp_source);
#endif

    struct {
        int32_t SIMRES[4] = { (int)XRES, (int)YRES, (int)ZRES };
        uint32_t data[GOL_RULE_COUNT * 2];
    } golRuleData;

    ssbosData = util::PersistentBuffer<2>(GL_SHADER_STORAGE_BUFFER, sizeof(gol_map), util::PBFlags::WRITE_ALT_READ);
    ssboRules = rlLoadShaderBuffer(sizeof(golRuleData), NULL, RL_STATIC_READ);

    for (std::size_t i = 0; i < GOL_RULE_COUNT; i++) {
        golRuleData.data[2*i]   = golRules[i].birth | (static_cast<uint64_t>(golRules[i].neighborhood) << 31);
        golRuleData.data[2*i+1] = golRules[i].survive;
    }

    rlUpdateShaderBuffer(ssboRules, &golRuleData, sizeof(golRuleData), 0);
    reset();
}

void SimulationGol::reset() {
    for (std::size_t i = 0; i < ssbosData.getBufferCount(); i++) {
        ssbosData.wait(i);
        std::fill(&ssbosData.get<uint8_t>(i)[0], &ssbosData.get<uint8_t>(i)[0] + (sizeof(gol_map) / sizeof(gol_map[0][0][0])), 0);
        ssbosData.lock(i);
    }
    memset(gol_map, 0, sizeof(gol_map));
}

void SimulationGol::dispatch() {
    ssbosData.wait(0);
    std::copy(
        &gol_map[0][0][0],
        &gol_map[0][0][0] + (sizeof(gol_map) / sizeof(gol_map[0][0][0])),
        &ssbosData.get<uint8_t>(0)[0]);
    ssbosData.lock(0);

    // Uncomment the two lines for timing the shader dispatch
    // util::GlTimeQuery query;

    rlEnableShader(golProgram);
    rlBindShaderBuffer(ssboRules, 0);
    rlBindShaderBuffer(ssbosData.getId(0), 1);
    rlBindShaderBuffer(ssbosData.getId(1), 2);
    // X is 4x as much since each invocation iterates 4 x values
    rlComputeShaderDispatch(std::ceil(XRES / 40.0), std::ceil(YRES / 10.0), std::ceil(ZRES / 10.0));
    rlDisableShader();

    // std::cout << query.timeElapsedMs() << " ms (gol)" << "\n";

    ssbosData.lock(1);
}

void SimulationGol::wait_and_get() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    ssbosData.wait(1);

    std::copy(
        &ssbosData.get<uint8_t>(1)[0],
        &ssbosData.get<uint8_t>(1)[0] + sizeof(gol_map) / sizeof(gol_map[0][0][0]),
        &gol_map[0][0][0]);

    ssbosData.advance_cycle();
    ssbosData.advance_cycle();
}