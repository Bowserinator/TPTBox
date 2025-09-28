#pragma once
#include "glad.h"
#include "rlgl.h"

/// RAII wrapper for GLSync
namespace util {
class GLSync {
public:
    GLSync() : m_sync(nullptr){};
    explicit GLSync(GLsync s) : m_sync(s) {}
    ~GLSync() { glDeleteSync(m_sync); }
    GLSync(const GLSync &)            = delete;
    GLSync &operator=(const GLSync &) = delete;
    GLSync &operator=(GLsync sync) {
        glDeleteSync(m_sync);
        m_sync = sync;
        return *this;
    }

    GLSync(GLSync &&o) noexcept : m_sync(o.m_sync) { o.m_sync = nullptr; }
    GLSync &operator=(GLSync &&o) noexcept {
        if (this != &o) {
            glDeleteSync(m_sync);
            m_sync   = o.m_sync;
            o.m_sync = nullptr;
        }
        return *this;
    }

    /// Accessors
    [[nodiscard]] GLsync get() const { return m_sync; }
    operator GLsync() const { return m_sync; }
    operator bool() const { return m_sync; }

    /// Call to indicate sync transfer has begun
    void lock() {
        if (m_sync) glDeleteSync(m_sync);
        m_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    /// Call to wait for a previous lock() to finish, if no previous lock()
    /// was issued will do nothing
    void wait() {
        if (!m_sync) return;
        while (true) {
            GLenum wait_return = glClientWaitSync(m_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
            if (wait_return == GL_ALREADY_SIGNALED || wait_return == GL_CONDITION_SATISFIED) return;
        }
    }

private:
    GLsync m_sync = nullptr;
};
} // namespace util
