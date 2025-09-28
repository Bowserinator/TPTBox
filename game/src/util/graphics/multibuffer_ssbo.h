#pragma once

#include "raylib.h"
#include "rlgl.h"
#include <algorithm>
#include <cstddef>
#include <utility>

namespace util {
class MultibufferSSBO {
public:
    MultibufferSSBO() : m_ssbos(nullptr), m_ssbo_count(0), m_cycle(0) {}
    MultibufferSSBO(const std::size_t size, const std::size_t bufferSize, int usage);
    ~MultibufferSSBO();

    MultibufferSSBO(const MultibufferSSBO &other) = delete;
    MultibufferSSBO(MultibufferSSBO &&other);
    MultibufferSSBO &operator=(const MultibufferSSBO &other) = delete;
    MultibufferSSBO &operator=(MultibufferSSBO &&other);

    std::size_t size() const { return m_ssbo_count; }

    void swap(MultibufferSSBO &other) noexcept {
        std::swap(m_ssbos, other.m_ssbos);
        std::swap(m_ssbo_count, other.m_ssbo_count);
        std::swap(m_cycle, other.m_cycle);
    }

    // Cycle forward all the ids
    void advance_cycle() { m_cycle++; }

    // Get SSBO at id, also taking into account cycle
    // Essentially returns ssbos[(i + cycle) % size()]
    unsigned int get(std::size_t i) const { return m_ssbos[(m_cycle + i) % m_ssbo_count]; }

private:
    unsigned int *m_ssbos;
    std::size_t m_ssbo_count;
    unsigned int m_cycle;
};
} // namespace util
