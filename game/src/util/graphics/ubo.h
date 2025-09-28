#pragma once

#include "raylib.h"
#include "rlgl.h"
#include "stdint.h"
#include <algorithm>
#include <cstring>
#include <glad.h>
#include <utility>

/**
 * @brief Uniform Buffer Object Writer
 * Why? Because UBO offsets can be unpredictable / require manual padding
 * This will get the offsets for each member of a uniform block for you
 *
 * Example:
 * layout(shared, binding = 4) uniform MyBlock {
 *   int X;
 *   float Y;
 * };
 *
 * auto writer = UBOWriter(myShaderProgramId, "MyBlock");
 * writer.write_member("X", 1);
 * writer.write_member("Y", 1.0f);
 * writer.upload(); // Will bind buffer!
 */
class UBOWriter {
public:
    /**
     * @brief Construct a new UBOWriter object
     *
     * @param program ID of shader program uniform buffer obj resides
     * @param UBOId ID of the UBO
     * @param uniform_block_name Name in the shader, ie uniform MyUBOName { would be "MyUBOName"
     */
    UBOWriter(const GLuint program, const GLuint UBOId, const char *uniform_block_name);

    ~UBOWriter() { destroy(); }
    UBOWriter(const UBOWriter &other)            = delete;
    UBOWriter &operator=(const UBOWriter &other) = delete;
    UBOWriter(UBOWriter &&other) {
        if (this != &other) swap(other);
    }
    UBOWriter &operator=(UBOWriter &&other) {
        if (this != &other) swap(other);
        return *this;
    }

    /**
     * @brief Write to a member in the class (in the temporary data array)
     *
     * @tparam T A pointer
     * @param member_name Name of the member to write to, ie "my_member" (does not need uniformBlockName)
     * @param value Pointer to start of new value to write to
     * @param size Size starting from the value pointer to copy from
     */
    template <class T> void write_member(const char *member_name, const T *value, std::size_t size) {
        memcpy(&m_data[0] + get_offset(member_name), value, size);
    }

    /**
     * @brief Write to a member in the class (in the temporary data array)
     *
     * @tparam T A non-pointer
     * @param member_name Name of the member to write to, ie "my_member" (does not need uniformBlockName)
     * @param value Value to write to the member, must be exactly the same type (ie if type is float you can't put a
     * double)
     */
    template <class T> void write_member(const char *member_name, const T &value) {
        memcpy(&m_data[0] + get_offset(member_name), &value, sizeof(value));
    }

    /// @brief Upload changes to the data array to the GPU, will bind the UBO buffer
    void upload() {
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_id);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, m_data_size_bytes, m_data);
    }

    /**
     * @brief Get size of the UBO struct in bytes, including padding / alignment
     * @return GLint
     */
    GLint size() const { return m_data_size_bytes; }

    void swap(UBOWriter &other) noexcept {
        std::swap(other.m_uniform_block_name, m_uniform_block_name);
        std::swap(other.m_program_id, m_program_id);
        std::swap(other.m_ubo_id, m_ubo_id);
        std::swap(other.m_data_size_bytes, m_data_size_bytes);
        std::swap(other.m_uniform_count, m_uniform_count);
        std::swap(other.m_data, m_data);
    }

private:
    const char *m_uniform_block_name;
    GLint m_program_id, m_ubo_id;
    GLint m_data_size_bytes, m_uniform_count; // Resolved in constructor with OpenGL call
    uint8_t *m_data;

    /**
     * @brief Get offset for member given name. Will query openGL
     *        every time ~15us but we don't expect you'll be
     *        modifying uniforms a lot anyways
     * @param member_name Name of the member
     * @return GLuint
     */
    GLuint get_offset(const char *member_name);

    /// Memory cleanup
    void destroy() {
        delete[] m_data;
        m_data = nullptr;
    }
};
