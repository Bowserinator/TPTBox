#include "persistent_buffer.h"
#include <glad.h>

using namespace util;

PersistentBuffer::PersistentBuffer(GLenum target, GLsizeiptr size):
    target(target)
{
    glGenBuffers(1, &buffId);
    glBindBuffer(target, buffId);
    auto flags = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    glBufferStorage(target, size, NULL, flags);
    ptr = glMapBufferRange(target, 0, size, flags);
}

PersistentBuffer::~PersistentBuffer() {
    glBindBuffer(target, buffId);
    glUnmapBuffer(target);
    glDeleteBuffers(1, &buffId);
}

void PersistentBuffer::lock() {
    if (syncObj) glDeleteSync(syncObj);
	syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

void PersistentBuffer::wait() {
    if (!syncObj) return;
    while (true) {
        GLenum waitReturn = glClientWaitSync(syncObj, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
        if (waitReturn == GL_ALREADY_SIGNALED || waitReturn == GL_CONDITION_SATISFIED)
            return;
    }
}
