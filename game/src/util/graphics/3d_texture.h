#pragma once
#include "glad.h"
#include "rlgl.h"
#include "util/common.h"
#include "util/graphics/gl_util.h"
#include "util/graphics/glsync.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace util {

enum class TextureFlags { NONE, READ, WRITE, READ_AND_WRITE };

class Async3dTexture {
    static GLuint gen_pbo(GLsizeiptr size, GLenum target, GLint usage) {
        GLuint pbo = 0;
        glGenBuffers(1, &pbo);
        if (pbo == 0) util::die(DEBUG_MSG("Failed to generate pixel buffer object"));
        glBindBuffer(target, pbo);
        glBufferData(target, size, nullptr, usage);
        glBindBuffer(target, 0);
        return pbo;
    }

    Async3dTexture(size_t W, size_t H, size_t D, TextureFlags tex_flags, GLint gpu_internal_format = GL_RGBA8,
                   GLenum cpu_format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE, GLint filter = GL_NEAREST,
                   GLint edge_mode = GL_CLAMP_TO_EDGE, int format_components = -1)
        : m_width(W), m_height(H), m_depth(D), m_cpu_format(cpu_format), m_cpu_type(type) {
        if (format_components <= 0) format_components = util::cpu_format_to_component_count(cpu_format);

        const GLsizeiptr tex_size_bytes = (GLsizeiptr)m_width * m_height * m_depth * format_components;
        glGenTextures(1, &m_tex);
        if (m_tex == 0) util::die(DEBUG_MSG("Failed to generate texture"));

        glBindTexture(GL_TEXTURE_3D, m_tex);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, edge_mode);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, edge_mode);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, edge_mode);
        glTexImage3D(GL_TEXTURE_3D, 0, gpu_internal_format, m_width, m_height, m_depth, 0, m_cpu_format, m_cpu_type,
                     nullptr);

        m_upload_pbo = gen_pbo(tex_size_bytes, GL_PIXEL_UNPACK_BUFFER, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_upload_pbo);
        m_download_pbo = gen_pbo(tex_size_bytes, GL_PIXEL_PACK_BUFFER, GL_STREAM_READ);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_download_pbo);

        auto flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        if (tex_flags == TextureFlags::READ || tex_flags == TextureFlags::READ) flags |= GL_MAP_READ_BIT;
        if (tex_flags == TextureFlags::WRITE || tex_flags == TextureFlags::READ) flags |= GL_MAP_WRITE_BIT;

        m_mapped = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, tex_size_bytes, flags);
        if (m_mapped == nullptr) util::die(DEBUG_MSG("glMapBufferRange returned nullptr"));
    }

    ~Async3dTexture() {
        glDeleteBuffers(1, &m_upload_pbo);
        glDeleteTextures(1, &m_tex);
        m_upload_pbo = 0;
        m_tex        = 0;
    }

    /// @brief Call after writing to internal buffer to upload and lock
    void upload_and_lock() {
        glBindTexture(GL_TEXTURE_3D, m_tex);
        glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, m_width, m_height, m_depth, m_cpu_format, m_cpu_type, nullptr);
        lock();
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0); // TODO? needed?
    }

    /// @brief Call to download and get data into mapped
    void download_and_wait() {
        glBindTexture(GL_TEXTURE_3D, m_tex);
        glGetTexImage(GL_TEXTURE_3D, 0, m_cpu_format, m_cpu_format, nullptr);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0); // TODO? needed?
        wait();
    }

    /// @brief Get access to internal mapped buffer for read/write
    template <class T> [[nodiscard]] T *get() { return (T *)m_mapped; }

    void lock() { m_sync.lock(); }
    void wait() { m_sync.wait(); }

private:
    GLuint m_tex          = 0;
    GLuint m_upload_pbo   = 0;
    GLuint m_download_pbo = 0;
    void *m_mapped        = nullptr;

    util::GLSync m_sync;
    size_t m_width, m_height, m_depth;
    GLenum m_cpu_format, m_cpu_type;
};

} // namespace util
