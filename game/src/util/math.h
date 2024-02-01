#ifndef UTIL_MATH_H
#define UTIL_MATH_H

#include <cmath>
#include "raymath.h"
#include "rand.h"
#include "vector_op.h"

template <class T>
concept arithmetic = std::integral<T> or std::floating_point<T>;

namespace util {
    /**
     * @brief Round a float fast. WARNING: pass in only positive values
     * @param r A POSITIVE float
     * @return constexpr unsigned int 
     */
    constexpr unsigned int roundf(const float r) {
        #ifdef DEBUG
        if (r < 0.0f) throw std::invalid_argument("Input to util::roundf must be non-negative, got " + std::to_string(r));
        #endif
        return (unsigned int)(r + 0.5f);
    }

    /**
     * @brief Clamp a float between two values
     * 
     * @param val Value
     * @param min Min of range
     * @param max Max of range
     * @return constexpr float 
     */
    constexpr float clampf(const float val, const float min, const float max) {
        const float tmp = val < min ? min : val;
        return tmp > max ? max : tmp;
    }
    constexpr int clamp(const int val, const int min, const int max) {
        const int tmp = val < min ? min : val;
        return tmp > max ? max : tmp;
    }

    template <class T> requires arithmetic<T>
    constexpr T clamp(const T val, const T min, const T max) {
        const T tmp = val < min ? min : val;
        return tmp > max ? max : tmp;
    }
    /**
     * @brief Ceil a float, but round negative values away from 0
     *        This is slightly different from std::ceil which always
     *        rounds in the positive direction
     * 
     * @param v Float value
     * @return constexpr float 
     */
    constexpr int ceil_proper(float v) {
        if (v < 0) return -std::ceil(-v);
        return std::ceil(v);
    }

    /**
     * @brief Return sqrt(x^2 + y^2), faster than std::hypot but
     *        no underflow/overflow protection guarantee
     * @param x 
     * @param y 
     * @return constexpr float 
     */
    template <class T> requires arithmetic<T>
    constexpr float hypot(T x, T y) {
        return std::sqrt(x * x + y * y);
    }
    template <class T> requires arithmetic<T>
    constexpr float hypot(T x, T y, T z) {
        return std::sqrt(x * x + y * y + z * z);
    }
    template <class T> requires arithmetic<T>
    constexpr float hypot(T x, T y, T z, T w) {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    /**
     * @brief Return the index of the largest in [a, b, c]
     * @param a 
     * @param b 
     * @param c 
     * @return constexpr int 
     */
    template <class T> requires arithmetic<T>
    constexpr int argmax3(T a, T b, T c) {
        a = std::abs(a);
        b = std::abs(b);
        c = std::abs(c);
        int idx = 0;

        if (b > a)
            idx = 1;
        if (c > b && c > a)
            idx = 2;
        return idx;
    }

    /**
     * @brief Return sign of value
     * @param a 
     * @return constexpr int -1 if negative, 0 if 0, 1 if pos
     */
    template <class T> requires arithmetic<T>
    constexpr int sign(T a) {
        if (a == 0) return 0;
        return a < 0 ? -1 : 1;
    }

    /**
     * @brief Radians to degrees
     * @param rad Radians
     * @return constexpr float 
     */
    constexpr float rad2deg(const float rad) {
        return rad * 180.0f / 3.1415926535897f;
    }

    /**
     * @brief Degrees to radians
     * @param deg Degrees
     * @return constexpr float 
     */
    constexpr float deg2rad(const float deg) {
        return deg * 3.1415926535897f / 180.0f;
    }

    /**
     * @brief Take the given transform matrix, remove the translation
     *        and scaling in-place. (Note: assumes there is no skew,
     *        otherwise it will leave behind the skew and rotation)
     * 
     *        Assume matrix is multiplied with points so translation is last
     *        column and not the other way around (being, translation is last row)
     * @param mat Matrix to reduce in place
     */
    inline void reduce_to_rotation(Matrix &mat) {
        mat.m12 = mat.m13 = mat.m14 = mat.m15 = 0.0f; // Remove translation column

        // Normalize first 3 columns
        // Blame raylib for having a variable for every matrix term
        auto sx = util::hypot(mat.m0, mat.m1, mat.m2, mat.m3);
        mat.m0 /= sx;
        mat.m1 /= sx;
        mat.m2 /= sx;
        mat.m3 /= sx;
        auto sy = util::hypot(mat.m4, mat.m5, mat.m6, mat.m7);
        mat.m4 /= sy;
        mat.m5 /= sy;
        mat.m6 /= sy;
        mat.m7 /= sy;
        auto sz = util::hypot(mat.m8, mat.m9, mat.m10, mat.m11);
        mat.m8 /= sy;
        mat.m9 /= sy;
        mat.m10 /= sy;
        mat.m11 /= sy;
    }

    /**
     * @brief Are two vectors similar? Aka all components vary by at
     *        most the given threshold
     * @param a 
     * @param b 
     * @param thresh 
     * @return true 
     * @return false 
     */
    inline bool vec3_similar(const Vector3 &a, const Vector3& b, const float thresh) {
        return fabsf(a.x - b.x) <= thresh && fabsf(a.y - b.y) <= thresh && fabsf(a.z - b.z) <= thresh;
    }

    inline Vector3 norm_vector(const Vector3 &v) {
        return v / (hypot(v.x, v.y, v.z) + 0.001f);
    }
    inline Vector2 norm_vector(const Vector2 &v) {
        return v / (hypot(v.x, v.y) + 0.001f);
    }
    inline Vector4 norm_vector(const Vector4 &v) {
        return v / (hypot(v.x, v.y, v.z, v.w) + 0.001f);
    }
}

#endif