#include "multibuffer_ssbo.h"

using namespace util;

MultibufferSSBO::MultibufferSSBO(const std::size_t ssboCount, const std::size_t bufferSize, int usage):
    ssboCount(ssboCount), cycle(0)
{
    ssbos = new unsigned int[ssboCount];
    for (auto i = 0; i < ssboCount; i++)
        ssbos[i] = rlLoadShaderBuffer(bufferSize, NULL, usage);
}

MultibufferSSBO::~MultibufferSSBO() {
    for (auto i = 0; i < ssboCount; i++)
        rlUnloadShaderBuffer(ssbos[i]);
    delete[] ssbos;
}

MultibufferSSBO::MultibufferSSBO(MultibufferSSBO &&other) {
    if (this != &other) swap(other);
}

MultibufferSSBO& MultibufferSSBO::operator=(MultibufferSSBO &&other) {
    if (&other != this) swap(other);
    return *this;
}
