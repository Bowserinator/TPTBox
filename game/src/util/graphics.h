#ifndef UTIL_GRAPHICS_H
#define UTIL_GRAPHICS_H

#include "rlgl.h"
#include "raymath.h"
#include "vector_op.h"

namespace util {
    // Quick assignment for shader vector values
    inline void set_shader_value(const Shader &shaderId, int propertyLoc, Vector2 vec) {
        float vecf[] = { vec.x, vec.y };
        SetShaderValue(shaderId, propertyLoc, vecf, SHADER_UNIFORM_VEC2);
    }
    inline void set_shader_value(const Shader &shaderId, int propertyLoc, Vector3 vec) {
        float vecf[] = { vec.x, vec.y, vec.z };
        SetShaderValue(shaderId, propertyLoc, vecf, SHADER_UNIFORM_VEC3);
    }
    inline void set_shader_value(const Shader &shaderId, int propertyLoc, Vector3T<int> vec) {
        int veci[] = { vec.x, vec.y, vec.z };
        SetShaderValue(shaderId, propertyLoc, veci, SHADER_UNIFORM_IVEC3);
    }
    inline void set_shader_value(const Shader &shaderId, int propertyLoc, Vector4 vec) {
        float vecf[] = { vec.x, vec.y, vec.z, vec.w };
        SetShaderValue(shaderId, propertyLoc, vecf, SHADER_UNIFORM_IVEC4);
    }
    inline void set_shader_value(const Shader &shaderId, int propertyLoc, float val) {
        SetShaderValue(shaderId, propertyLoc, &val, SHADER_UNIFORM_FLOAT);
    }
    inline void set_shader_value(const Shader &shaderId, int propertyLoc, int val) {
        SetShaderValue(shaderId, propertyLoc, &val, SHADER_UNIFORM_INT);
    }
}

#endif