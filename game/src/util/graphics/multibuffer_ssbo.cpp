#include "multibuffer_ssbo.h"

using namespace util;

MultibufferSSBO::MultibufferSSBO(const std::size_t ssbo_count, const std::size_t buffer_size, int usage):
    m_ssbo_count(ssbo_count), m_cycle(0)
{
    m_ssbos = new unsigned int[ssbo_count];
    for (std::size_t i = 0; i < ssbo_count; i++)
        m_ssbos[i] = rlLoadShaderBuffer(buffer_size, NULL, usage);
}

MultibufferSSBO::~MultibufferSSBO() {
    for (std::size_t i = 0; i < m_ssbo_count; i++)
        rlUnloadShaderBuffer(m_ssbos[i]);
    delete[] m_ssbos;
}

MultibufferSSBO::MultibufferSSBO(MultibufferSSBO &&other) {
    if (this != &other) swap(other);
}

MultibufferSSBO& MultibufferSSBO::operator=(MultibufferSSBO &&other) {
    if (&other != this) swap(other);
    return *this;
}
