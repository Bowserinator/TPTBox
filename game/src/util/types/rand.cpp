#include "rand.h"
#include <cstdlib>
#include <ctime>

#include "../vector_op.h"
#include "../math.h"
#include "raymath.h"

uint64_t RNG::next() {
    s[0] *= 0xda942042e4dd58b5;
    return s[0] >> 64;
}

unsigned int RNG::gen() {
    return next() & 0x7FFFFFFF;
}

int RNG::rand() {
    return static_cast<int>(gen());
}

unsigned int RNG::operator()() {
    return next() & 0xFFFFFFFF;
}

int RNG::between(int lower, int upper) {
    unsigned int r = next();
    return static_cast<int>(r % ((unsigned int)(upper) - (unsigned int)(lower) + 1U)) + lower;
}

bool RNG::chance(int numerator, unsigned int denominator) {
    if (numerator < 0)
       return false;
    return next() % denominator < static_cast<unsigned int>(numerator);
}

float RNG::uniform01() {
    return static_cast<float>(next()&0xFFFFFFFF) / (float)0xFFFFFFFF;
}

float RNG::uniform(float lower, float upper) {
    return uniform01() * (upper - lower) + lower;
}

Vector3 RNG::rand_perpendicular_vector(const Vector3 ray) {
    constexpr float RANGE = 0.57735026919f; // sqrt(1/3), should cap max magnitude at 1.0f

    // Project random vector to first and subtract component
    Vector3 randv{ uniform(-RANGE, RANGE), uniform(-RANGE, RANGE), uniform(-RANGE, RANGE) };
    randv -= Vector3DotProduct(randv, ray) / Vector3DotProduct(ray, ray) * ray;
    return randv;
}

Vector3 RNG::rand_norm_vector() {
    constexpr float RANGE = 1.0f;
    Vector3 randv{ uniform(-RANGE, RANGE), uniform(-RANGE, RANGE), uniform(-RANGE, RANGE) };
    return util::norm_vector(randv);
}

RNG::RNG() {
    s[0] = time(NULL);
}

void RNG::seed(unsigned int sd) {
    s[0] = sd;
}
