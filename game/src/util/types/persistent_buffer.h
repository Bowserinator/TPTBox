#ifndef UTIL_PERSISTENT_BUFFER_H
#define UTIL_PERSISTENT_BUFFER_H

#include <glad.h>
#include "rlgl.h"
#include <cstddef>
#include <algorithm>
#include <stdexcept>

namespace util {
    enum class PBFlags { NONE, READ, WRITE, READ_AND_WRITE, WRITE_ALT_READ, READ_ALT_WRITE };

    template <std::size_t bufferCount>
    class PersistentBuffer {
    public:
        // Create a persistent buffer
        // @tparam bufferCount: Number of buffers to use (ie 3 = triple buffering), manually cycle with .cycle()
        // @param target: Target buffer binding ie GL_SHADER_STORAGE_BUFFER
        // @param size: Size of the buffer in bytes
        // @param rwFlag: Additional flags, see the Flags enum
        PersistentBuffer(GLenum target, GLsizeiptr size, PBFlags rwFlag);
        PersistentBuffer(): PersistentBuffer(0, 0, PBFlags::NONE) {};
        ~PersistentBuffer();

        PersistentBuffer(const PersistentBuffer &other) = delete;
        PersistentBuffer(PersistentBuffer &&other);
        PersistentBuffer &operator=(const PersistentBuffer &other) = delete;
        PersistentBuffer &operator=(PersistentBuffer &&other);

        std::size_t getBufferCount() const { return bufferCount; }
        std::size_t size() const { return _size; }

        void swap(PersistentBuffer &other) noexcept {
            std::swap(target, other.target);
            std::swap(buffsId, other.buffsId);
            std::swap(syncObjs, other.syncObjs);
            std::swap(cycle, other.cycle);
            std::swap(ptrs, other.ptrs);
            std::swap(_size, other._size);
        }

        void lock(std::size_t i = 0);
        void wait(std::size_t i = 0);

        // Cycle forward all the ids
        void advance_cycle() { cycle = (cycle + 1) % bufferCount; }

        // Get buffer at id, also taking into account cycle
        // Essentially returns buffsId[(i + cycle) % size()]
        GLuint getId(std::size_t i) const { return buffsId[(cycle + i) % bufferCount]; }

        // Get pointer at index i, respecting cycle
        template <class T = void>
        T * get(std::size_t i) { return (T*)ptrs[(cycle + i) % bufferCount]; }

        // Get target binding
        GLenum getTarget() const { return target; }

        void ** ptrs = nullptr;
    private:
        GLenum target;
        GLsync * syncObjs = nullptr;
        GLuint * buffsId = nullptr;
        std::size_t _size;
        unsigned int cycle = 0;
    };

    
    template <std::size_t bufferCount>
    PersistentBuffer<bufferCount>::PersistentBuffer(GLenum target, GLsizeiptr size, PBFlags flag):
        target(target), _size(size)
    {
        if (size > 0 && bufferCount > 0) {
            buffsId = new GLuint[bufferCount];
            ptrs = new void*[bufferCount];
            syncObjs = new GLsync[bufferCount];

            GLbitfield rwFlag = 0;
            if (flag == PBFlags::READ || flag == PBFlags::READ_AND_WRITE)
                rwFlag |= GL_MAP_READ_BIT;
            if (flag == PBFlags::WRITE || flag == PBFlags::READ_AND_WRITE)
                rwFlag |= GL_MAP_WRITE_BIT;

            glGenBuffers(bufferCount, buffsId);

            for (int i = 0; i < bufferCount; i++) {
                if (flag == PBFlags::WRITE_ALT_READ)
                    rwFlag = i % 2 == 0 ? GL_MAP_WRITE_BIT : GL_MAP_READ_BIT;
                else if (flag == PBFlags::READ_ALT_WRITE)
                    rwFlag = i % 2 == 1 ? GL_MAP_WRITE_BIT : GL_MAP_READ_BIT;

                glBindBuffer(target, buffsId[i]);
                auto flags = rwFlag | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
                glBufferStorage(target, size, NULL, flags);

                ptrs[i] = glMapBufferRange(target, 0, size, flags);
                #ifdef DEBUG
                if (!ptrs[i]) throw std::runtime_error("Failed to map buffer range");
                #endif
                syncObjs[i] = 0;
            }
        }
    }

    template <std::size_t bufferCount>
    PersistentBuffer<bufferCount>::~PersistentBuffer() {
        if (bufferCount && _size) {
            for (int i = 0; i < bufferCount; i++) {
                glBindBuffer(target, buffsId[i]);
                glUnmapBuffer(target);
            }
            glDeleteBuffers(bufferCount, buffsId);
        }
        delete[] buffsId;
        delete[] ptrs;
        delete[] syncObjs;

        buffsId = nullptr;
        ptrs = nullptr;
        syncObjs = nullptr;
    }

    template <std::size_t bufferCount>
    PersistentBuffer<bufferCount>::PersistentBuffer(PersistentBuffer &&other) {
        if (this != &other) swap(other);
    }

    template <std::size_t bufferCount>
    PersistentBuffer<bufferCount>& PersistentBuffer<bufferCount>::operator=(PersistentBuffer &&other) {
        if (&other != this) swap(other);
        return *this;
    }

    template <std::size_t bufferCount>
    void PersistentBuffer<bufferCount>::lock(std::size_t i) {
        i = (cycle + i) % bufferCount;
        if (syncObjs[i]) glDeleteSync(syncObjs[i]);
        syncObjs[i] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    template <std::size_t bufferCount>
    void PersistentBuffer<bufferCount>::wait(std::size_t i) {
        i = (cycle + i) % bufferCount;
        if (!syncObjs[i]) return;
        while (true) {
            GLenum waitReturn = glClientWaitSync(syncObjs[i], GL_SYNC_FLUSH_COMMANDS_BIT, 1);
            if (waitReturn == GL_ALREADY_SIGNALED || waitReturn == GL_CONDITION_SATISFIED)
                return;
        }
    }
}

#endif