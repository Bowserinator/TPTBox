#pragma once

#include "glad.h"
#include "rlgl.h"
#include "util/graphics/glsync.h"
#include "util/common.h"

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace util {
enum class PBFlags { NONE, READ, WRITE, READ_AND_WRITE, WRITE_ALT_READ, READ_ALT_WRITE };

template <std::size_t buffer_count> class PersistentBuffer {
public:
    // Create a persistent buffer
    // @tparam bufferCount: Number of buffers to use (ie 3 = triple buffering), manually cycle with .cycle()
    // @param target: Target buffer binding ie GL_SHADER_STORAGE_BUFFER
    // @param size: Size of the buffer in bytes
    // @param rwFlag: Additional flags, see the Flags enum
    PersistentBuffer(GLenum target, GLsizeiptr size, PBFlags rw_flag);
    PersistentBuffer() : PersistentBuffer(0, 0, PBFlags::NONE){};
    ~PersistentBuffer();

    PersistentBuffer(const PersistentBuffer &other) = delete;
    PersistentBuffer(PersistentBuffer &&other);
    PersistentBuffer &operator=(const PersistentBuffer &other) = delete;
    PersistentBuffer &operator=(PersistentBuffer &&other);

    std::size_t get_buffer_count() const { return buffer_count; }
    std::size_t size() const { return m_size; }

    void swap(PersistentBuffer &other) noexcept {
        std::swap(m_target, other.m_target);
        std::swap(m_buffs_id, other.m_buffs_id);
        std::swap(m_sync_objs, other.m_sync_objs);
        std::swap(m_cycle, other.m_cycle);
        std::swap(m_ptrs, other.m_ptrs);
        std::swap(m_size, other.m_size);
    }

    void lock(std::size_t i = 0);
    void wait(std::size_t i = 0);

    // Cycle forward all the ids
    void advance_cycle() { m_cycle = (m_cycle + 1) % buffer_count; }

    // Get buffer at id, also taking into account cycle
    // Essentially returns buffsId[(i + cycle) % size()]
    GLuint get_id(std::size_t i) const { return m_buffs_id[(m_cycle + i) % buffer_count]; }

    // Get pointer at index i, respecting cycle
    template <class T = void> T *get(std::size_t i) { return (T *)m_ptrs[(m_cycle + i) % buffer_count]; }

    // Get target binding
    GLenum get_target() const { return m_target; }

    void **m_ptrs = nullptr;

private:
    GLenum m_target;
    util::GLSync *m_sync_objs = nullptr;
    GLuint *m_buffs_id        = nullptr;
    std::size_t m_size;
    unsigned int m_cycle = 0;
};

template <std::size_t bufferCount>
PersistentBuffer<bufferCount>::PersistentBuffer(GLenum target, GLsizeiptr size, PBFlags flag)
    : m_target(target), m_size(size) {
    if (size > 0 && bufferCount > 0) {
        m_buffs_id  = new GLuint[bufferCount];
        m_ptrs      = new void *[bufferCount];
        m_sync_objs = new util::GLSync[bufferCount];

        GLbitfield rw_flag = 0;
        if (flag == PBFlags::READ || flag == PBFlags::READ_AND_WRITE) rw_flag |= GL_MAP_READ_BIT;
        if (flag == PBFlags::WRITE || flag == PBFlags::READ_AND_WRITE) rw_flag |= GL_MAP_WRITE_BIT;

        glGenBuffers(bufferCount, m_buffs_id);

        for (std::size_t i = 0; i < bufferCount; i++) {
            if (flag == PBFlags::WRITE_ALT_READ)
                rw_flag = i % 2 == 0 ? GL_MAP_WRITE_BIT : GL_MAP_READ_BIT;
            else if (flag == PBFlags::READ_ALT_WRITE)
                rw_flag = i % 2 == 1 ? GL_MAP_WRITE_BIT : GL_MAP_READ_BIT;

            glBindBuffer(target, m_buffs_id[i]);
            auto flags = rw_flag | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
            glBufferStorage(target, size, NULL, flags);

            m_ptrs[i] = glMapBufferRange(target, 0, size, flags);
#ifdef DEBUG
            if (!m_ptrs[i]) util::die(DEBUG_MSG("Failed to map buffer range"));
#endif
        }
    }
}

template <std::size_t buffer_count> PersistentBuffer<buffer_count>::~PersistentBuffer() {
    if (buffer_count && m_size) {
        for (std::size_t i = 0; i < buffer_count; i++) {
            glBindBuffer(m_target, m_buffs_id[i]);
            glUnmapBuffer(m_target);
        }
        glDeleteBuffers(buffer_count, m_buffs_id);
    }
    delete[] m_buffs_id;
    delete[] m_ptrs;
    delete[] m_sync_objs;

    m_buffs_id  = nullptr;
    m_ptrs      = nullptr;
    m_sync_objs = nullptr;
}

template <std::size_t bufferCount> PersistentBuffer<bufferCount>::PersistentBuffer(PersistentBuffer &&other) {
    if (this != &other) swap(other);
}

template <std::size_t bufferCount>
PersistentBuffer<bufferCount> &PersistentBuffer<bufferCount>::operator=(PersistentBuffer &&other) {
    if (&other != this) swap(other);
    return *this;
}

template <std::size_t bufferCount> void PersistentBuffer<bufferCount>::lock(std::size_t i) {
    i = (m_cycle + i) % bufferCount;
    m_sync_objs[i].lock();
}

template <std::size_t bufferCount> void PersistentBuffer<bufferCount>::wait(std::size_t i) {
    i = (m_cycle + i) % bufferCount;
    m_sync_objs[i].wait();
}
} // namespace util
