#ifndef UTIL_TYPES_RAND_H_
#define UTIL_TYPES_RAND_H_

#include "raylib.h"

#include <stdint.h>
#include <array>

class RNG {
public:
    using State = std::array<__uint128_t, 1>;

private:
    State s;
    uint64_t next() noexcept;

    static const std::array<Vector3, 128> NORMAL_VECTORS;

public:
    // Same as gen()
    unsigned int operator()() noexcept;

    /**
     * @brief Generate random uint
     * @return unsigned int 
     */
    unsigned int gen() noexcept;

    /**
     * @brief Generate random int
     * @return int 
     */
    int rand() noexcept;

    /**
     * @brief Generate a random number between lower
     *        and upper INCLUSIVE
     * @param lower 
     * @param upper 
     * @return int 
     */
    int between(const int lower, const int upper) noexcept;

    /**
     * @brief Return true if rand01() < numerator / denom
     *        Note: may not be implemented as such
     * @param numerator
     * @param denominator
     * @return bool
     */
    bool chance(const unsigned int numerator, const unsigned int denominator) noexcept;

    /**
     * @brief Random float between 0 and 1
     * @return float 
     */
    float uniform01() noexcept;

    /**
     * @brief Return random float between two values inclusive
     * @param lower 
     * @param upper 
     * @return float 
     */
    float uniform(const float lower, const float upper) noexcept;

    /**
     * @brief Returns a random vector3 orthogonal to the given one
     *        with a magnitude between 0 and 1.0. Does not guarantee the result
     *        is non-zero
     * @param ray Given vector, must have non-zero magnitude
     * @param rng RNG instance
     * @return Vector3 
     */
    Vector3 rand_perpendicular_vector(const Vector3 ray);

    /**
     * @brief Generate a random vector with magnitude 1.0f
     * @return Vector3 
     */
    Vector3 rand_unit_vector();

    RNG();
    void seed(unsigned int sd);

    void state(State ns) {
        s = ns;
    }

    State state() const {
        return s;
    }
};

#endif // UTIL_TYPES_RAND_H_
