#ifndef BRUSHES_H
#define BRUSHES_H

#include "raylib.h"
#include "raymath.h"
#include "../../util/vector_op.h"

#include <string>
#include <functional>
#include <array>
#include <cmath>

// xyz (-size/2 to size/2), size, rotation
#define BRUSH_MAP_ARGS Vector3 pos, Vector3 size, Vector3 rotation

class Brush {
public:
    using brushMap = std::function<bool(BRUSH_MAP_ARGS)>;

    Brush(const std::string &name, brushMap map, bool solid): name(name), map(map), solid(solid) {};

    const std::string name;
    const brushMap map;
    const bool solid;
};

const std::array<Brush, 6> BRUSHES({
    Brush("Rectangular Prism",
        [](BRUSH_MAP_ARGS) { return true; }, true),

    Brush("Sphere", 
        [](BRUSH_MAP_ARGS) {
            Vector3 sqr = Vector3{ (pos.x * pos.x), (pos.y * pos.y), (pos.z * pos.z) }; 
            return sqr.x / (size.x * size.x / 4.0f)
                + sqr.y / (size.y * size.y / 4.0f)
                + sqr.z / (size.z * size.z / 4.0f) <= 1.0f;
        }, true),

    Brush("Cylinder", 
        [](BRUSH_MAP_ARGS) {
            return (pos.x * pos.x) / (size.x * size.x / 4.0f) +
                (pos.z * pos.z) / (size.z * size.z / 4.0f) <= 1.0f;
        }, true),

    Brush("Square Pyramid", 
        [](BRUSH_MAP_ARGS) {
            return std::max(std::abs(pos.x / size.x), std::abs(pos.z / size.z)) <= (0.5f - pos.y / size.y) / 2;
        }, true),

    Brush("Cone", 
        [](BRUSH_MAP_ARGS) {
            return (pos.x * pos.x) / (size.x * size.x / 4.0f) +
                (pos.z * pos.z) / (size.z * size.z / 4.0f) <= (0.5f - pos.y / size.y) * (0.5f - pos.y / size.y);
        }, true),

    Brush("Tetrahedron", 
        [](BRUSH_MAP_ARGS) {
            Vector3 pos2{ (pos.x) / size.x * 2.0f, (pos.y) / size.y * 2.0f, (pos.z) / size.z * 2.0f, };
            const auto plane = [&](Vector3 c, Vector3 n) { return Vector3DotProduct(pos2 - c, n); };

            constexpr float f = 0.57735;
            constexpr float e = 1.0f;
            const float a = plane(Vector3{e,e,e}, Vector3{-f,f,f}); 
            const float b = plane(Vector3{e,-e,-e}, Vector3{f,-f,f});
            const float c = plane(Vector3{-e,e,-e}, Vector3{f,f,-f});
            const float d = plane(Vector3{-e,-e,e}, Vector3{-f,-f,-f});
            return std::max(std::max(a, b), std::max(c,d)) <= 0.0f;
        }, true),
});

#endif