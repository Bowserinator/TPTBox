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
    using previewRender = std::function<void(Vector3, Vector3)>;

    Brush(const std::string &name, brushMap map, previewRender preview): name(name), map(map), preview(preview) {};

    const std::string name;
    const brushMap map;
    const previewRender preview;
};

const std::array<Brush, 6> BRUSHES({
    Brush("Rectangular Prism",
        [](BRUSH_MAP_ARGS) { return true; },
        [](Vector3 pos, Vector3 size) { DrawCubeWires(pos, size.x, size.y, size.z, WHITE); }),

    Brush("Sphere", 
        [](BRUSH_MAP_ARGS) {
            Vector3 sqr = Vector3{ (pos.x * pos.x), (pos.y * pos.y), (pos.z * pos.z) }; 
            return sqr.x / (size.x * size.x / 4.0f)
                + sqr.y / (size.y * size.y / 4.0f)
                + sqr.z / (size.z * size.z / 4.0f) <= 1.0f;
        },
        [](Vector3 pos, Vector3 size) {
            // TODO: technically ellipsoid
            DrawCubeWires(pos, size.x, size.y, size.z, GRAY);
            DrawSphereWires(pos, size.x / 2, 20, 20, WHITE);
        }),

    Brush("Cylinder", 
        [](BRUSH_MAP_ARGS) {
            return (pos.x * pos.x) / (size.x * size.x / 4.0f) +
                (pos.z * pos.z) / (size.z * size.z / 4.0f) <= 1.0f;
        },
        [](Vector3 pos, Vector3 size) {
            // TODO: elliupsoid
            // DrawCubeWires(pos, size.x, size.y, size.z, GRAY);
            pos.y -= size.y / 2;
            DrawCylinderWires(pos, size.x / 2, size.x / 2, size.y, 20, WHITE);
        }),

    Brush("Square Pyramid", 
        [](BRUSH_MAP_ARGS) {
            return std::max(std::abs(pos.x / size.x), std::abs(pos.z / size.z)) <= (0.5f - pos.y / size.y) / 2;
        },
        [](Vector3 pos, Vector3 size) {
            // DrawCubeWires(pos, size.x, size.y, size.z, GRAY);
            pos.y -= size.y / 2;
            DrawLine3D(pos - Vector3{size.x / 2, 0.0f, size.z / 2}, pos - Vector3{-size.x / 2, 0.0f, size.z / 2}, WHITE);
            DrawLine3D(pos - Vector3{size.x / 2, 0.0f, size.z / 2}, pos - Vector3{size.x / 2, 0.0f, -size.z / 2}, WHITE);
            DrawLine3D(pos + Vector3{size.x / 2, 0.0f, size.z / 2}, pos + Vector3{-size.x / 2, 0.0f, size.z / 2}, WHITE);
            DrawLine3D(pos + Vector3{size.x / 2, 0.0f, size.z / 2}, pos + Vector3{size.x / 2, 0.0f, -size.z / 2}, WHITE);

            DrawLine3D(pos + Vector3{size.x / 2, 0.0f, size.z / 2}, pos + Vector3{0.0f, size.y, 0.0f}, WHITE);
            DrawLine3D(pos + Vector3{-size.x / 2, 0.0f, size.z / 2}, pos + Vector3{0.0f, size.y, 0.0f}, WHITE);
            DrawLine3D(pos + Vector3{size.x / 2, 0.0f, -size.z / 2}, pos + Vector3{0.0f, size.y, 0.0f}, WHITE);
            DrawLine3D(pos + Vector3{-size.x / 2, 0.0f, -size.z / 2}, pos + Vector3{0.0f, size.y, 0.0f}, WHITE);
        }),

    Brush("Cone", 
        [](BRUSH_MAP_ARGS) {
            return (pos.x * pos.x) / (size.x * size.x / 4.0f) +
                (pos.z * pos.z) / (size.z * size.z / 4.0f) <= (0.5f - pos.y / size.y) * (0.5f - pos.y / size.y);
        },
        [](Vector3 pos, Vector3 size) {
            DrawCubeWires(pos, size.x, size.y, size.z, GRAY);
        }),

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
        },
        [](Vector3 pos, Vector3 size) {
            // DrawCubeWires(pos, size.x, size.y, size.z, GRAY);
            Vector3 vertices[] = {
                pos + Vector3{size.x / 2, size.y / 2, size.z / 2},
                pos + Vector3{-size.x / 2, -size.y / 2, size.z / 2},
                pos + Vector3{size.x / 2, -size.y / 2, -size.z / 2},
                pos + Vector3{-size.x / 2, size.y / 2, -size.z / 2}
            };
            DrawLine3D(vertices[0], vertices[1], WHITE);
            DrawLine3D(vertices[0], vertices[2], WHITE);
            DrawLine3D(vertices[1], vertices[2], WHITE);
            DrawLine3D(vertices[0], vertices[3], WHITE);
            DrawLine3D(vertices[1], vertices[3], WHITE);
            DrawLine3D(vertices[2], vertices[3], WHITE);
        }),
});

#endif