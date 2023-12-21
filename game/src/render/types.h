#ifndef RENDER_TYPES_H
#define RENDER_TYPES_H

#include "raylib.h"
#include "raymath.h"
#include "../util/vector_op.h"

#include <cmath>
#include <vector>

struct Plane {
    Vector3 normal;
    float distance;

    // Signed distance of sphere to plane TODO doc
    bool sphereDisPlane(float sx, float sy, float sz) {
        return normal.x * sx + normal.y * sy + normal.z * sz + distance;
    }
};

Plane FromNumbers(const Vector4 &numbers) {
    Vector3 abc{numbers.x, numbers.y, numbers.z};
    auto mag = Vector3Length(abc);
    abc /= mag;

    Plane p;
    p.normal = abc;
    p.distance = numbers.w / mag;
    return p;
}

std::vector<Plane> getFrustum(const Camera3D &camera) {
    std::vector<Plane> frustum(6);
    Matrix vp = GetCameraMatrix(camera);

    const Vector4 row1{ vp.m0, vp.m4, vp.m8, vp.m12 };
    const Vector4 row2{ vp.m1, vp.m5, vp.m9, vp.m13 };
    const Vector4 row3{ vp.m2, vp.m6, vp.m10, vp.m14 };
    const Vector4 row4{ vp.m3, vp.m7, vp.m11, vp.m15 };

    frustum[0] = FromNumbers(row4 + row1);
    frustum[1] = FromNumbers(row4 - row1);
    frustum[2] = FromNumbers(row4 + row2);
    frustum[3] = FromNumbers(row4 - row2);
    frustum[4] = FromNumbers(row4 + row3);
    frustum[5] = FromNumbers(row4 - row3);
    return frustum;
}


#endif