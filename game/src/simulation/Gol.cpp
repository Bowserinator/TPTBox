#include "Gol.h"
#include "rlgl.h"

#include "render/constants.h"
#include "util/graphics.h"
#include "util/graphics/shader.h"
#include "util/string.h"
#include "util/types/gl_time_query.h"
#include "util/vector_op.h"

#include <cstring>
#include <glad.h>

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

const std::size_t GOL_RULE_COUNT = 14;

const GOLRule golRules[GOL_RULE_COUNT] = {
    GOLRule("678", "678 678. 6-8/6-8/M/3", gRule({6, 7, 8}), gRule({6, 7, 8}), RGBA(0x6002EEFF),
            GOLRule::Neighborhood::MOORE, 2),
    GOLRule("445", "4/4/5/M", gRule({4}), gRule({4}), RGBA(0xF44336FF), GOLRule::Neighborhood::MOORE, 4),
    GOLRule("AMBA", "Amoeba. 9-26/5-7,12,13,15/5/M",
            gRule({9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26}),
            gRule({5, 6, 7, 12, 13, 15}), RGBA(0xAD1457FF), GOLRule::Neighborhood::MOORE, 4),
    GOLRule("BULD", "Builder. 2,6,9/4,6,8,9/10/M", gRule({2, 6, 9}), gRule({4, 6, 8, 9}), RGBA(0xAB47BCFF),
            GOLRule::Neighborhood::MOORE, 9),
    GOLRule("CLD1", "Clouds 1. 13-26/13-14,17-19/2/M", gRule({13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26}),
            gRule({13, 14, 17, 18, 19}), RGBA(0xE040FBFF), GOLRule::Neighborhood::MOORE, 9),
    GOLRule("CRYS", "Crystal Growth 1. 0-6/1,3/2/VN", gRule({0, 1, 2, 3, 4, 5, 6}), gRule({1, 3}), RGBA(0x1565C0FF),
            GOLRule::Neighborhood::NEUMANN, 1),
    GOLRule("CRY2", "Crystal Growth 2. 1-2/1,3/5/VN", gRule({1, 2}), gRule({1, 3}), RGBA(0xFFA726FF),
            GOLRule::Neighborhood::NEUMANN, 4),
    GOLRule("PYRO", "Pyroclastic. 4-7/6-8/10/M", gRule({4, 5, 6, 7}), gRule({6, 7, 8}), RGBA(0x00BFA5FF),
            GOLRule::Neighborhood::MOORE, 9),
    GOLRule("SLOW", "Slow Decay. 1,4,8,11,13-26/13-26/5/M",
            gRule({1, 4, 8, 11, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26}),
            gRule({13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26}), RGBA(0x76FF03FF),
            GOLRule::Neighborhood::MOORE, 4),
    GOLRule("MORE", "More Structures. 7-26/4/4/M",
            gRule({7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26}), gRule({4}),
            RGBA(0xAFB42BFF), GOLRule::Neighborhood::MOORE, 3),
    GOLRule("PULS", "Pulse Waves. 3/1-3/10/M", gRule({3}), gRule({1, 2, 3}), RGBA(0xFFC400FF),
            GOLRule::Neighborhood::MOORE, 9),
    GOLRule("REPL", "Single Point Replicator. /1/2/M", gRule({}), gRule({1}), RGBA(0xB0BEC5FF),
            GOLRule::Neighborhood::MOORE, 1),
    GOLRule("SYMM", "Symmetry. /2/10/M", gRule({}), gRule({2}), RGBA(0xF57F17FF), GOLRule::Neighborhood::MOORE, 9),
    GOLRule("VNEU", "Von Neumann Builder. 1-3/1,4-5/5/VN", gRule({1, 2, 3}), gRule({4, 5}), RGBA(0xC6FF00FF),
            GOLRule::Neighborhood::NEUMANN, 4)};

// GOL SIM
// -----------------------------
SimulationGol::~SimulationGol() { rlUnloadShaderBuffer(ssbos_rules); }

void SimulationGol::init() {
#include "../../resources/shaders/generated/gol.comp.h"
    gol_shader = util::TPBComputeShader(gol_comp_source);

    struct {
        int32_t SIMRES[4] = {(int)XRES, (int)YRES, (int)ZRES};
        uint32_t data[GOL_RULE_COUNT * 2];
    } gol_rule_data;

    ssbos_data  = util::PersistentBuffer<2>(GL_SHADER_STORAGE_BUFFER, sizeof(gol_map), util::PBFlags::WRITE_ALT_READ);
    ssbos_rules = rlLoadShaderBuffer(sizeof(gol_rule_data), NULL, RL_STATIC_READ);

    for (std::size_t i = 0; i < GOL_RULE_COUNT; i++) {
        gol_rule_data.data[2 * i]     = golRules[i].birth | (static_cast<uint64_t>(golRules[i].neighborhood) << 31);
        gol_rule_data.data[2 * i + 1] = golRules[i].survive;
    }

    rlUpdateShaderBuffer(ssbos_rules, &gol_rule_data, sizeof(gol_rule_data), 0);
    reset();
}

void SimulationGol::reset() {
    for (std::size_t i = 0; i < ssbos_data.getBufferCount(); i++) {
        ssbos_data.wait(i);
        std::fill(&ssbos_data.get<gol_map_t>(i)[0],
                  &ssbos_data.get<gol_map_t>(i)[0] + (sizeof(gol_map) / sizeof(gol_map[0][0][0])), 0);
        ssbos_data.lock(i);
    }
    memset(gol_map, 0, sizeof(gol_map));
}

void SimulationGol::dispatch() {
    ssbos_data.wait(0);
    std::copy(&gol_map[0][0][0], &gol_map[0][0][0] + (sizeof(gol_map) / sizeof(gol_map[0][0][0])),
              &ssbos_data.get<gol_map_t>(0)[0]);
    ssbos_data.lock(0);

    // Uncomment the two lines for timing the shader dispatch
    // util::GlTimeQuery query;

    rlEnableShader(gol_shader.id());
    rlBindShaderBuffer(ssbos_rules, 0);
    rlBindShaderBuffer(ssbos_data.getId(0), 1);
    rlBindShaderBuffer(ssbos_data.getId(1), 2);
    // X is 4x as much since each invocation iterates 4 x values
    rlComputeShaderDispatch(std::ceil(XRES / 64.0), std::ceil(YRES / 8.0), std::ceil(ZRES / 8.0));
    rlDisableShader();

    // std::cout << query.timeElapsedMs() << " ms (gol)" << "\n";

    ssbos_data.lock(1);
}

void SimulationGol::wait_and_get() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    ssbos_data.wait(1);

    std::copy(&ssbos_data.get<gol_map_t>(1)[0],
              &ssbos_data.get<gol_map_t>(1)[0] + sizeof(gol_map) / sizeof(gol_map[0][0][0]), &gol_map[0][0][0]);

    gol_map[0][0][0] = 0; // Placeholder spot for edge padding
    ssbos_data.advance_cycle();
    ssbos_data.advance_cycle();
}
