#include "rand.h"
#include <cstdlib>
#include <ctime>

#include "../vector_op.h"
#include "../math.h"
#include "raymath.h"

uint64_t RNG::next() noexcept {
    s[0] *= 0xda942042e4dd58b5;
    return s[0] >> 64;
}

unsigned int RNG::gen() noexcept {
    return next() & 0x7FFFFFFF;
}

int RNG::rand() noexcept {
    return static_cast<int>(gen());
}

unsigned int RNG::operator()() noexcept {
    return next() & 0xFFFFFFFF;
}

int RNG::between(const int lower, const int upper) noexcept {
    unsigned int r = next();
    return static_cast<int>(r % ((unsigned int)(upper) - (unsigned int)(lower) + 1U)) + lower;
}

bool RNG::chance(const unsigned int numerator, const unsigned int denominator) noexcept {
    return next() % denominator < numerator;
}

float RNG::uniform01() noexcept {
    return static_cast<float>(next() & 0xFFFFFFFF) / (float)0xFFFFFFFF;
}

float RNG::uniform(const float lower, const float upper) noexcept {
    return uniform01() * (upper - lower) + lower;
}

Vector3 RNG::rand_perpendicular_vector(const Vector3 ray) {
    constexpr float RANGE = 0.57735026919f; // sqrt(1/3), should cap max magnitude at 1.0f

    // Project random vector to first and subtract component
    Vector3 randv{ uniform(-RANGE, RANGE), uniform(-RANGE, RANGE), uniform(-RANGE, RANGE) };
    randv -= Vector3DotProduct(randv, ray) / Vector3DotProduct(ray, ray) * ray;
    return randv;
}

Vector3 RNG::rand_unit_vector() {
    return NORMAL_VECTORS[next() % NORMAL_VECTORS.size()];
}

RNG::RNG() {
    s[0] = time(NULL);
}

void RNG::seed(unsigned int sd) {
    s[0] = sd;
}

// Big list of random vectors on a sphere
// NOLINTBEGIN
const std::array<Vector3, 128> RNG::NORMAL_VECTORS({
    Vector3{-0.15386f,-0.90017f,0.40747f},Vector3{0.15386f,0.90017f,-0.40747f},Vector3{-0.79368f,0.57632f,-0.19473f},Vector3{0.79368f,-0.57632f,0.19473f},Vector3{-0.48724f,0.87112f,-0.06124f},Vector3{0.48724f,-0.87112f,0.06124f},Vector3{-0.36326f,0.85575f,0.36842f},Vector3{0.36326f,-0.85575f,-0.36842f},Vector3{-0.10792f,-0.6517f,-0.75076f},Vector3{0.10792f,0.6517f,0.75076f},Vector3{-0.51275f,0.81181f,0.27937f},Vector3{0.51275f,-0.81181f,-0.27937f},Vector3{0.97599f,0.03065f,0.21564f},Vector3{-0.97599f,-0.03065f,-0.21564f},Vector3{0.7533f,-0.60498f,-0.25795f},Vector3{-0.7533f,0.60498f,0.25795f},Vector3{-0.8565f,-0.29405f,-0.4242f},Vector3{0.8565f,0.29405f,0.4242f},Vector3{-0.82244f,0.05586f,0.5661f},Vector3{0.82244f,-0.05586f,-0.5661f},Vector3{0.41408f,-0.84371f,0.34161f},Vector3{-0.41408f,0.84371f,-0.34161f},Vector3{0.79897f,-0.55486f,-0.23192f},Vector3{-0.79897f,0.55486f,0.23192f},Vector3{-0.77192f,0.32034f,-0.54911f},Vector3{0.77192f,-0.32034f,0.54911f},Vector3{0.10827f,-0.07641f,-0.99118f},Vector3{-0.10827f,0.07641f,0.99118f},Vector3{-0.2831f,-0.9399f,-0.1909f},Vector3{0.2831f,0.9399f,0.1909f},Vector3{-0.80448f,-0.37668f,-0.45927f},Vector3{0.80448f,0.37668f,0.45927f},Vector3{0.48264f,0.86973f,0.10307f},Vector3{-0.48264f,-0.86973f,-0.10307f},Vector3{-0.85663f,-0.15071f,0.49342f},Vector3{0.85663f,0.15071f,-0.49342f},Vector3{0.58466f,-0.50486f,-0.63504f},Vector3{-0.58466f,0.50486f,0.63504f},Vector3{-0.97384f,-0.20088f,0.10623f},Vector3{0.97384f,0.20088f,-0.10623f},Vector3{0.07256f,0.2628f,-0.96212f},Vector3{-0.07256f,-0.2628f,0.96212f},Vector3{0.96163f,0.27434f,0.00299f},Vector3{-0.96163f,-0.27434f,-0.00299f},Vector3{0.94486f,0.26622f,-0.19068f},Vector3{-0.94486f,-0.26622f,0.19068f},Vector3{0.3097f,-0.60558f,0.73305f},Vector3{-0.3097f,0.60558f,-0.73305f},Vector3{0.25137f,-0.60437f,-0.75601f},Vector3{-0.25137f,0.60437f,0.75601f},Vector3{-0.47813f,0.01606f,-0.87814f},Vector3{0.47813f,-0.01606f,0.87814f},Vector3{0.41433f,-0.31883f,-0.85245f},Vector3{-0.41433f,0.31883f,0.85245f},Vector3{-0.69796f,0.70767f,0.10981f},Vector3{0.69796f,-0.70767f,-0.10981f},Vector3{-0.1556f,0.62813f,-0.76239f},Vector3{0.1556f,-0.62813f,0.76239f},Vector3{0.5035f,0.46993f,0.72502f},Vector3{-0.5035f,-0.46993f,-0.72502f},Vector3{0.24537f,-0.59421f,-0.76597f},Vector3{-0.24537f,0.59421f,0.76597f},Vector3{-0.82461f,-0.55627f,0.10291f},Vector3{0.82461f,0.55627f,-0.10291f},Vector3{-0.7917f,0.01755f,-0.61066f},Vector3{0.7917f,-0.01755f,0.61066f},Vector3{0.76548f,0.03366f,-0.64257f},Vector3{-0.76548f,-0.03366f,0.64257f},Vector3{0.58983f,-0.2713f,0.76059f},Vector3{-0.58983f,0.2713f,-0.76059f},Vector3{0.47833f,0.79905f,-0.3643f},Vector3{-0.47833f,-0.79905f,0.3643f},Vector3{-0.82329f,-0.01604f,-0.5674f},Vector3{0.82329f,0.01604f,0.5674f},Vector3{-0.09985f,0.2052f,-0.97361f},Vector3{0.09985f,-0.2052f,0.97361f},Vector3{0.17977f,-0.46364f,-0.86759f},Vector3{-0.17977f,0.46364f,0.86759f},Vector3{-0.11512f,0.93277f,-0.34161f},Vector3{0.11512f,-0.93277f,0.34161f},Vector3{0.71981f,0.686f,-0.10622f},Vector3{-0.71981f,-0.686f,0.10622f},Vector3{0.81557f,-0.57745f,-0.03748f},Vector3{-0.81557f,0.57745f,0.03748f},Vector3{-0.85107f,0.13152f,0.50832f},Vector3{0.85107f,-0.13152f,-0.50832f},Vector3{-0.55721f,0.37454f,-0.7411f},Vector3{0.55721f,-0.37454f,0.7411f},Vector3{-0.89906f,-0.03221f,-0.43665f},Vector3{0.89906f,0.03221f,0.43665f},Vector3{-0.2375f,0.96998f,0.0523f},Vector3{0.2375f,-0.96998f,-0.0523f},Vector3{0.13103f,-0.01958f,-0.99119f},Vector3{-0.13103f,0.01958f,0.99119f},Vector3{0.35658f,-0.89628f,-0.26368f},Vector3{-0.35658f,0.89628f,0.26368f},Vector3{0.47567f,0.73357f,-0.4854f},Vector3{-0.47567f,-0.73357f,0.4854f},Vector3{0.59999f,-0.00025f,0.80001f},Vector3{-0.59999f,0.00025f,-0.80001f},Vector3{-0.95381f,-0.17932f,-0.24102f},Vector3{0.95381f,0.17932f,0.24102f},Vector3{-0.06872f,0.4665f,0.88185f},Vector3{0.06872f,-0.4665f,-0.88185f},Vector3{0.04631f,-0.20872f,-0.97688f},Vector3{-0.04631f,0.20872f,0.97688f},Vector3{-0.55986f,0.82694f,0.05223f},Vector3{0.55986f,-0.82694f,-0.05223f},Vector3{-0.9443f,-0.0646f,0.32269f},Vector3{0.9443f,0.0646f,-0.32269f},Vector3{-0.64588f,-0.17855f,0.74226f},Vector3{0.64588f,0.17855f,-0.74226f},Vector3{-0.80081f,-0.54045f,-0.25811f},Vector3{0.80081f,0.54045f,0.25811f},Vector3{0.05574f,0.95185f,-0.30147f},Vector3{-0.05574f,-0.95185f,0.30147f},Vector3{0.55625f,0.82554f,0.09523f},Vector3{-0.55625f,-0.82554f,-0.09523f},Vector3{0.99401f,-0.06451f,0.08821f},Vector3{-0.99401f,0.06451f,-0.08821f},Vector3{-0.2481f,-0.87488f,0.41597f},Vector3{0.2481f,0.87488f,-0.41597f},Vector3{0.22684f,0.97182f,0.06413f},Vector3{-0.22684f,-0.97182f,-0.06413f},Vector3{0.14237f,-0.33933f,-0.92983f},Vector3{-0.14237f,0.33933f,0.92983f},Vector3{0.0283f,0.33501f,-0.94179f},Vector3{-0.0283f,-0.33501f,0.94179f}
});
// NOLINTEND
