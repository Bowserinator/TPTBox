#pragma once
#include "glad.h"
#include "util/common.h"

namespace util {
[[nodiscard]] constexpr int cpu_format_to_component_count(const GLenum cpu_format) {
    switch (cpu_format) {
    case GL_RED:
    case GL_RED_INTEGER:  return 1;
    case GL_RG:
    case GL_RG_INTEGER:   return 2;
    case GL_RGB:
    case GL_BGR:
    case GL_RGB_INTEGER:
    case GL_BGR_INTEGER:  return 3;
    case GL_RGBA:
    case GL_BGRA:
    case GL_RGBA_INTEGER:
    case GL_BGRA_INTEGER: return 4;
    default:              util::die(DEBUG_MSG(std::format("Unknown format {}", cpu_format)));
    }
    return -1;
}
} // namespace util
