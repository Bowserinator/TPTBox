#ifndef UTIL_TYPES_UBO_H_
#define UTIL_TYPES_UBO_H_

#include "raylib.h"
#include "stdint.h"
#include "rlgl.h"
#include <glad.h>
#include <cstring>
#include <algorithm>
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
     * @param uniformBlockName Name in the shader, ie uniform MyUBOName { would be "MyUBOName"
     */
    UBOWriter(const GLuint program, const GLuint UBOId, const char * uniformBlockName);

    ~UBOWriter() { destroy(); }
    UBOWriter(const UBOWriter &other) = delete;
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
     * @param memberName Name of the member to write to, ie "my_member" (does not need uniformBlockName)
     * @param value Pointer to start of new value to write to
     * @param size Size starting from the value pointer to copy from
     */
    template <class T>
    void write_member(const char * memberName, const T* value, std::size_t size) {
        memcpy(&data[0] + get_offset(memberName), value, size);
    }

    /**
     * @brief Write to a member in the class (in the temporary data array)
     * 
     * @tparam T A non-pointer
     * @param memberName Name of the member to write to, ie "my_member" (does not need uniformBlockName)
     * @param value Value to write to the member, must be exactly the same type (ie if type is float you can't put a double)
     */
    template <class T>
    void write_member(const char * memberName, const T &value) {
        memcpy(&data[0] + get_offset(memberName), &value, sizeof(value));
    }

    /**
     * @brief Upload changes to the data array to the GPU, will bind the UBO buffer
     */
    void upload() {
        glBindBuffer(GL_UNIFORM_BUFFER, UBOId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, dataSizeBytes, data);
    }

    /**
     * @brief Get size of the UBO struct in bytes, including padding / alignment
     * @return GLint 
     */
    GLint size() const { return dataSizeBytes; }

    void swap(UBOWriter &other) noexcept {
        std::swap(other.uniformBlockName, uniformBlockName);
        std::swap(other.programId, programId);
        std::swap(other.UBOId, UBOId);
        std::swap(other.dataSizeBytes, dataSizeBytes);
        std::swap(other.uniformCount, uniformCount);
        std::swap(other.data, data);
    }

private:
    const char * uniformBlockName;
    GLint programId, UBOId;
    GLint dataSizeBytes, uniformCount; // Resolved in constructor with OpenGL call
    uint8_t * data;

    /**
     * @brief Get offset for member given name. Will query openGL
     *        every time ~15us but we don't expect you'll be
     *        modifying uniforms a lot anyways
     * @param memberName Name of the member
     * @return GLuint 
     */
    GLuint get_offset(const char * memberName);

    /**
     * @brief Memory cleanup
     */
    void destroy() {
        delete[] data;
        data = nullptr;
    }
};

#endif // UTIL_TYPES_UBO_H_
