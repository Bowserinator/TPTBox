#pragma once

#include "raylib.h"

#define USE_SIMD // Enable to use SIMD versions of code, assumes SSE4

#ifdef USE_SIMD
#include "../../libs/simde/x86/sse.h"
#endif

// Will use non-simd version if not SIMD enabled
// _ip suffix:   in place
// _full suffix: 2nd operand takes <n> arguments as well

namespace simd_util {
    /// SIMD optimized float clamp (modifies in directly)
    extern void clamp4f_ip(float &a, float &b, float &c, float &d, const float min, const float max);
    extern void clamp3f_ip(float &a, float &b, float &c, const float min, const float max);
    extern void clamp3f_ip_full(float &a, float &b, float &c, const float min1, float min2, float min3, float max1, float max2, float max3);

    // SIMD multiply by same value (modifies in directly)
    extern Vector4 mul4f(float a, float b, float c, float d, const float multiplier);
    extern Vector3 mul3f(float a, float b, float c, const float multiplier);
    extern void mul4f_ip(float &a, float &b, float &c, float &d, const float multiplier);
    extern void mul3f_ip(float &a, float &b, float &c, const float multiplier);

    extern Vector4 add4f_full(float a, float b, float c, float d, float x, float y, float z, float w);
    extern Vector3 add3f_full(float a, float b, float c, float x, float y, float z);
} // namespace simd_util
