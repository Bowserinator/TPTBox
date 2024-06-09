#ifndef SIMULATION_SIMULATIONDEF_H_
#define SIMULATION_SIMULATIONDEF_H_

#include <stdint.h>

using coord_t = uint8_t;
using signed_coord_t = int16_t;
using ElementType = unsigned int;

// Must be signed, negative values are used for pfree
// and error codes in some functions
using part_id = int32_t;
using pmap_id = int32_t; // This is ID and type merged into 1 32 bit value

// Unsigned
using part_type = uint16_t;
using part_type_s = int32_t; // Must be larger than part_type

// Pmap and photons store positive ids in this format:
// [TYP 10 bits][ID 22 bits]
constexpr uint32_t PMAP_ID_BITS = 22;
constexpr uint16_t PT_NUM = 1 << (32 - PMAP_ID_BITS); // Max number of elements possible

#define ID(r) (r & ((1 << PMAP_ID_BITS) - 1))
#define TYP(r) (r >> PMAP_ID_BITS)
#define PMAP(t, i) (((uint32_t)t << PMAP_ID_BITS) | (uint32_t)i)

constexpr uint16_t ELEMENT_COUNT = __GLOBAL_ELEMENT_COUNT;

// Should be <= 256, larger values may break it especially for morton codes
// in util and the part fragment shader
constexpr unsigned int XRES = 200;
constexpr unsigned int YRES = 200;
constexpr unsigned int ZRES = 200;
constexpr unsigned int NPARTS = XRES * YRES * ZRES;

constexpr unsigned int SHADOW_MAP_SCALE = 1; // Mostly unused, needs to be set in shader as well
constexpr unsigned int SHADOW_MAP_X = (XRES + ZRES) / SHADOW_MAP_SCALE;
constexpr unsigned int SHADOW_MAP_Y = (YRES + ZRES) / SHADOW_MAP_SCALE;

constexpr float R_ZERO_C = 273.15f;
constexpr float R_TEMP = R_ZERO_C + 22.0f;
constexpr float MAX_VELOCITY = 50.0f;

constexpr float MIN_TEMP = 0.0f;
constexpr float MAX_TEMP = 99999.0f;

constexpr float operator""_C(long double tempC) { return tempC + R_ZERO_C; }

constexpr bool BOUNDS_CHECK(int x, int y, int z) {
    return x > 0 && x < (int)(XRES - 1) && y > 0 && y < (int)(YRES - 1) && z > 0 && z < (int)(ZRES - 1);
}
constexpr bool REVERSE_BOUNDS_CHECK(int x, int y, int z) {
    return x < 1 || x >= (int)(XRES - 1) || y < 1 || y >= (int)(YRES - 1) || z < 1 || z >= (int)(ZRES - 1);
}
constexpr uint32_t FLAT_IDX(coord_t x, coord_t y, coord_t z) {
    return x + y * XRES + z * (XRES * YRES);
}

// A 'fast' random number generator for heat conduction
// Basically sample from a randomly shuffled array of all possible bytes
constexpr uint8_t HEAT_RANDOM_BYTES[] = {180, 225, 22, 187, 20, 252, 115, 190, 237, 10, 219, 26, 133, 196, 48, 169, 165, 179, 151, 27, 60, 28, 97, 90, 111, 88, 228, 185, 212, 236, 215, 13, 39, 8, 155, 127, 23, 253, 136, 41, 15, 217, 118, 211, 243, 142, 152, 57, 121, 241, 183, 83, 70, 75, 113, 45, 242, 77, 69, 4, 67, 29, 63, 71, 153, 93, 147, 56, 5, 122, 34, 226, 74, 216, 86, 78, 193, 109, 209, 105, 18, 232, 244, 206, 11, 229, 43, 186, 36, 117, 246, 96, 106, 255, 53, 159, 173, 125, 2, 141, 144, 197, 62, 108, 64, 177, 157, 25, 7, 175, 208, 234, 104, 101, 55, 80, 94, 98, 191, 204, 1, 162, 170, 176, 103, 240, 79, 102, 166, 116, 158, 205, 249, 247, 161, 178, 192, 52, 200, 140, 172, 174, 42, 68, 31, 130, 65, 76, 33, 58, 99, 54, 221, 202, 201, 137, 87, 37, 194, 0, 66, 112, 184, 44, 238, 100, 251, 156, 47, 38, 120, 126, 207, 107, 32, 92, 149, 245, 254, 91, 135, 124, 167, 19, 129, 214, 154, 81, 128, 50, 199, 223, 84, 233, 150, 49, 46, 188, 119, 198, 85, 250, 21, 235, 195, 24, 35, 203, 132, 72, 146, 218, 61, 181, 220, 51, 163, 210, 17, 123, 9, 12, 189, 14, 3, 40, 168, 230, 227, 138, 114, 110, 160, 224, 95, 131, 231, 239, 148, 6, 171, 143, 139, 145, 213, 16, 182, 73, 222, 89, 248, 134, 164, 59, 82, 30};
constexpr bool HEAT_CONDUCT_CHANCE(int frame_count, coord_t x, coord_t y, coord_t z, int heatConduct) {
    return HEAT_RANDOM_BYTES[(frame_count + x + 11 * y + 191 * z) & 0xFF] <= heatConduct;
}

enum class PartSwapBehavior: uint8_t {
    NOOP = 0,
    SWAP = 1,
    OCCUPY_SAME = 2,
    SPECIAL = 3,
    NOT_EVALED_YET = 4 // Sentinel value, do not use in normal cases
};

// Normal = create if not empty
// Brush  = normal, with special brush rules
// Force  = Create regardless if occupied
enum class PartCreateMode { NORMAL, BRUSH, FORCE };

namespace PartErr {
    constexpr part_id ALREADY_OCCUPIED = -1;
    constexpr part_id PARTS_FULL = -3;
    constexpr part_id NOT_ALLOWED = -4;
};

namespace Transition {
    constexpr part_type_s NONE = -1;
    constexpr part_type_s TO_CTYPE = -2;
};

constexpr int MIN_CASUALITY_RADIUS = 4; // Width of each slice = 2 * this
constexpr int MAX_SIM_THREADS = ZRES / (4 * MIN_CASUALITY_RADIUS); // Threads = number of slices / 2
constexpr unsigned int GRAPHICS_LOCK_BLOCK_SIZE = 32;

// Block size should be a power of 2 for cache alignment
constexpr unsigned int CAUSALITY_ARRAY_BLOCK_SIZE = 1024;

#endif // SIMULATION_SIMULATIONDEF_H_
