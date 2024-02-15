#ifndef UTIL_MULTIBUFFER_SSBO
#define UTIL_MULTIBUFFER_SSBO

#include "raylib.h"
#include "rlgl.h"
#include <cstddef>
#include <algorithm>

namespace util {
    class MultibufferSSBO {
    public:
        MultibufferSSBO(): ssboCount(0), ssbos(nullptr), cycle(0) {}
        MultibufferSSBO(const std::size_t size, const std::size_t bufferSize, int usage);
        ~MultibufferSSBO();

        MultibufferSSBO(const MultibufferSSBO &other) = delete;
        MultibufferSSBO(MultibufferSSBO &&other);
        MultibufferSSBO &operator=(const MultibufferSSBO &other) = delete;
        MultibufferSSBO &operator=(MultibufferSSBO &&other);

        std::size_t size() const { return ssboCount; }

        void swap(MultibufferSSBO &other) noexcept {
            std::swap(ssbos, other.ssbos);
            std::swap(ssboCount, other.ssboCount);
            std::swap(cycle, other.cycle);
        }

        // Cycle forward all the ids
        void advance_cycle() { cycle++; }

        // Get SSBO at id, also taking into account cycle
        // Essentially returns ssbos[(i + cycle) % size()]
        unsigned int get(std::size_t i) const { return ssbos[(cycle + i) % ssboCount]; }
    private:
        unsigned int * ssbos;
        std::size_t ssboCount;
        unsigned int cycle;
    };
}

#endif