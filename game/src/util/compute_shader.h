#ifndef UTIL_COMPUTE_SHADER_H
#define UTIL_COMPUTE_SHADER_H

#include <glad.h>
#include "glm.hpp"
#include <iostream>

struct ComputeShaderInput {
    const unsigned int sizex;
    const unsigned int sizey;
    const unsigned int sizez;
    const unsigned int flat_size;
    const unsigned int bind_idx1, bind_idx2;
};

/**
 * @brief A general ComputeShader class with two buffers that can be
 *        written to / read from
 * @tparam T Type of each element in buffer1 sent to the compute shader
 * @tparam S Type of each element in buffer2 sent to the compute shader
 * @tparam BINDING_INDEX Index (number 0-7) to use, must match the number in
 *         the compute shader, ie layout(binding=4) buffer ..., the number is 4
 *         in this case. The 0-7 range is always guaranteed by OpenGL but your
 *         system may allow higher indices. Also recommended to use numbers
 *         not in the 0-3 range as they may be reserved by other buffers
 */
template <class T, class S = T>
class ComputeShader3D {
public:
    unsigned int id;
    unsigned int bind_idx1, bind_idx2;

    ComputeShader3D(const char * code, const ComputeShaderInput &in):
            bind_idx1(in.bind_idx1), bind_idx2(in.bind_idx2) {
        work_size = glm::uvec3(in.sizex, in.sizey, in.sizez);
        flat_size = in.flat_size < 0 ? work_size.x * work_size.y * work_size.z : in.flat_size;

        // Compile shader
        unsigned int shader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(shader, 1, &code, NULL);
        glCompileShader(shader);

        // Check shader actually compiled
        int status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            char errorLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, &errorLog[0]);
            throw std::invalid_argument(TextFormat("Compute shader failed to compile:\n%s", errorLog));
        }

        // Create program
        id = glCreateProgram();
        glAttachShader(id, shader);
        glLinkProgram(id);
        glDeleteShader(shader);

        // Create buffer for storage
        glGenBuffers(1, &ssbo1);
        glGenBuffers(1, &ssbo2);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo1);
        glBufferData(GL_SHADER_STORAGE_BUFFER, flat_size * sizeof(T), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);
        glBufferData(GL_SHADER_STORAGE_BUFFER, flat_size * sizeof(S), NULL, GL_STATIC_DRAW);
    }

    ~ComputeShader3D() {
        glDeleteProgram(id);
    }

    void set_buff1(T* values) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo1);
        glBufferData(GL_SHADER_STORAGE_BUFFER, flat_size * sizeof(T), values, GL_STATIC_DRAW);
    }

    void set_buff2(S* values) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);
        glBufferData(GL_SHADER_STORAGE_BUFFER, flat_size * sizeof(S), values, GL_STATIC_DRAW);
    }

    void use_and_dispatch() {
        glUseProgram(id);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bind_idx1, ssbo1);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bind_idx2, ssbo2);
        glDispatchCompute(work_size.x, work_size.y, work_size.z);
    }

    void wait() {
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void write_buff1_to(T * out) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo1);
        T *ptr = (T*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        std::copy(&ptr[0], &ptr[flat_size], out);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    void write_buff2_to(S * out) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);
        S *ptr = (S*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        std::copy(&ptr[0], &ptr[flat_size], out);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

private:
    glm::uvec3 work_size;
    GLuint ssbo1, ssbo2;
    unsigned int flat_size;
};

#endif