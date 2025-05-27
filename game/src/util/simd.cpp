#include "simd.h"
#include "math.h"


void simd_util::clamp4f_ip(float &a, float &b, float &c, float &d, const float min, const float max) {
#ifdef USE_SIMD
    float out[4];
    _mm_store_ps(out,
        _mm_min_ps(
            _mm_max_ps(_mm_set_ps(d, c, b, a), _mm_set_ps1(min)),
            _mm_set_ps1(max)
        ));
    a = out[0];
    b = out[1];
    c = out[2];
    d = out[3];
#else
    a = util::clampf(a, min, max);
    b = util::clampf(b, min, max);
    c = util::clampf(c, min, max);
    d = util::clampf(d, min, max);
#endif
}

void simd_util::clamp3f_ip(float &a, float &b, float &c, const float min, const float max) {
#ifdef USE_SIMD
    float out[4];
    _mm_store_ps(out,
        _mm_min_ps(
            _mm_max_ps(_mm_set_ps(0.0f, c, b, a), _mm_set_ps1(min)),
            _mm_set_ps1(max)
        ));
    a = out[0];
    b = out[1];
    c = out[2];
#else
    a = util::clampf(a, min, max);
    b = util::clampf(b, min, max);
    c = util::clampf(c, min, max);
#endif
}

void simd_util::clamp3f_ip_full(float &a, float &b, float &c, const float min1, float min2, float min3,
        float max1, float max2, float max3) {
#ifdef USE_SIMD
    float out[4];
    _mm_store_ps(out,
        _mm_min_ps(
            _mm_max_ps(_mm_set_ps(0.0f, c, b, a), _mm_set_ps(0.0f, min3, min2, min1)),
            _mm_set_ps(0.0f, max3, max2, max1)
        ));
    a = out[0];
    b = out[1];
    c = out[2];
#else
    a = util::clampf(a, min1, max1);
    b = util::clampf(b, min2, max2);
    c = util::clampf(c, min3, max3);
#endif
}

Vector4 simd_util::mul4f(float a, float b, float c, float d, const float multiplier) {
#ifdef USE_SIMD
    float out[4];
    _mm_store_ps(out, _mm_mul_ps(_mm_set_ps(d, c, b, a), _mm_set_ps1(multiplier)));
    return Vector4{ out[0], out[1], out[2], out[3] };
#else
    return Vector4{
        a * multiplier,
        b * multiplier,
        c * multiplier,
        d * multiplier
    };
#endif
}

void simd_util::mul4f_ip(float &a, float &b, float &c, float &d, const float multiplier) {
#ifdef USE_SIMD
    float out[4];
    _mm_store_ps(out, _mm_mul_ps(_mm_set_ps(d, c, b, a), _mm_set_ps1(multiplier)));
    a = out[0]; b = out[1];
    c = out[2]; d = out[3];
#else
    a *= multiplier;
    b *= multiplier;
    c *= multiplier;
    d *= multiplier;
#endif
}

void simd_util::mul3f_ip(float &a, float &b, float &c, const float multiplier) {
#ifdef USE_SIMD
    float out[4];
    _mm_store_ps(out, _mm_mul_ps(_mm_set_ps(0.0f, c, b, a), _mm_set_ps1(multiplier)));
    a = out[0]; b = out[1];
    c = out[2];
#else
    a *= multiplier;
    b *= multiplier;
    c *= multiplier;
#endif
}

Vector3 simd_util::mul3f(float a, float b, float c, const float multiplier) {
#ifdef USE_SIMD
    float out[4];
    _mm_store_ps(out, _mm_mul_ps(_mm_set_ps(0.0, c, b, a), _mm_set_ps1(multiplier)));
    return Vector3{ out[0], out[1], out[2] };
#else
    return Vector3{
        a * multiplier,
        b * multiplier,
        c * multiplier
    };
#endif
}

Vector4 simd_util::add4f_full(float a, float b, float c, float d, float x, float y, float z, float w) {
#ifdef USE_SIMD
    float out[4];
    _mm_store_ps(out, _mm_add_ps(_mm_set_ps(d, c, b, a), _mm_set_ps(w, z, y, x)));
    return Vector4{ out[0], out[1], out[2], out[3] };
#else
    return Vector4{
        a + x,
        b + y,
        c + z,
        d + w
    };
#endif
}

Vector3 simd_util::add3f_full(float a, float b, float c, float x, float y, float z) {
#ifdef USE_SIMD
    float out[4];
    _mm_store_ps(out, _mm_add_ps(_mm_set_ps(0.0, c, b, a), _mm_set_ps(0.0, z, y, x)));
    return Vector3{ out[0], out[1], out[2] };
#else
    return Vector3{
        a + x,
        b + y,
        c + z
    };
#endif
}
