#ifndef UTIL_UTIL_H
#define UTIL_UTIL_H

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
     * @brief Returns a random vector3 orthogonal to the given one
     *        with a magnitude between 0 and 1.0. Does not guarantee the result
     *        is non-zero
     * @param ray Given vector, must have non-zero magnitude
     * @param rng RNG instance
     * @return Vector3 
     */
    inline Vector3 rand_perpendicular_vector(const Vector3 ray, RNG &rng) {
        constexpr float RANGE = 0.57735026919f; // sqrt(1/3), should cap max magnitude at 1.0f

        // Project random vector to first and subtract component
        Vector3 randv{ rng.uniform(-RANGE, RANGE), rng.uniform(-RANGE, RANGE), rng.uniform(-RANGE, RANGE) };
        randv -= Vector3DotProduct(randv, ray) / Vector3DotProduct(ray, ray) * ray;
        return randv;
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
}

#endif