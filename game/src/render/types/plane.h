#ifndef RENDER_TYPES_PLANE_H_
#define RENDER_TYPES_PLANE_H_

#include "raymath.h"
#include "../../util/vector_op.h"

#include <iostream>

class Plane {
public:
    Vector3 normal;
    float distance;

    Plane(): normal{1.0f, 0.0f, 0.0f}, distance(0.0f) {}

    /**
     * @brief Construct a Plane of the equation normal \dot <x,y,z> + distance = 0
     * @param normal Normal vector (should be normalized to unit length)
     * @param distance z offset from the origin
     */
    Plane(Vector3 &normal, float distance): normal{normal}, distance{distance} {}

    /**
     * @brief Construct a Plane from a vector <normal_x, normal_y, normal_z, distance>
     *        Normalizes the normal vector for you
     * @param numbers The vector of the normal vector and offset, does not need to be normalized
     */
    Plane(const Vector4 &numbers) {
        Vector3 norm{numbers.x, numbers.y, numbers.z};
        auto mag = Vector3Length(norm);
        normal = norm / mag;
        distance = numbers.w / mag;
    }

    Plane(const Plane &other) {
        normal = Vector3{other.normal.x, other.normal.y, other.normal.z};
        distance = other.distance;
    }
    Plane& operator=(const Plane& other) {
        normal = Vector3{other.normal.x, other.normal.y, other.normal.z};
        distance = other.distance;
        return *this;
    }

    /**
     * @brief Get signed distance from point to plane (positive in direction
     *        of normal vector ("above" the plane), else negative)
     * @param x, y, z
     * @return Signed distance to plane 
     */
    float distanceToPlane(float x, float y, float z) const {
        return normal.x * x + normal.y * y + normal.z * z + distance;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Plane &plane) {
    os << "n: " << plane.normal << " d: " << plane.distance;
    return os;
}

#endif // RENDER_TYPES_PLANE_H_
