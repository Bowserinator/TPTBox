#ifndef UTIL_PERSISTENT_BUFFER_H
#define UTIL_PERSISTENT_BUFFER_H

#include <glad.h>
#include "rlgl.h"

namespace util {
    class PersistentBuffer {
    public:
        PersistentBuffer(GLenum target, GLsizeiptr size);
        ~PersistentBuffer();

        void lock();
        void wait();

        GLuint getId() const { return buffId; }

        void * ptr;
        const GLenum target;
    private:
        GLsync syncObj = 0;
        GLuint buffId;
    };
}

#endif