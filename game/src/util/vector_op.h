#ifndef OPERATORS_H
#define OPERATORS_H

// Adds operator overloads to raylib vectors

#include "raylib.h"
#include <iostream>

// Vector2 operator overload
inline std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
    os << '<' << vec.x << ", " << vec.y << '>';
    return os;
}

inline bool operator==(const Vector2 &lhs, const Vector2 &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}
inline bool operator!=(const Vector2 &lhs, const Vector2 &rhs) {
    return !(lhs == rhs);
}

inline Vector2 &operator+=(Vector2 &lhs, const Vector2 &rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}
inline Vector2 operator+(Vector2 lhs, const Vector2 &rhs) {
    lhs += rhs;
    return lhs;
}

inline Vector2 &operator-=(Vector2 &lhs, const Vector2 &rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}
inline Vector2 operator-(Vector2 lhs, const Vector2 &rhs) {
    lhs -= rhs;
    return lhs;
}

inline Vector2 &operator*=(Vector2 &lhs, const float &rhs) {
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}
inline Vector2 operator*(Vector2 lhs, const float &rhs) {
    lhs *= rhs;
    return lhs;
}

inline Vector2 &operator/=(Vector2 &lhs, const float &rhs) {
    lhs.x /= rhs;
    lhs.y /= rhs;
    return lhs;
}
inline Vector2 operator/(Vector2 lhs, const float &rhs) {
    lhs /= rhs;
    return lhs;
}



// Vector3 operator overload
inline std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
    os << '<' << vec.x << ", " << vec.y << ", " << vec.z << '>';
    return os;
}

inline bool operator==(const Vector3 &lhs, const Vector3 &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}
inline bool operator!=(const Vector3 &lhs, const Vector3 &rhs) {
    return !(lhs == rhs);
}

inline Vector3 &operator+=(Vector3 &lhs, const Vector3 &rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}
inline Vector3 operator+(Vector3 lhs, const Vector3 &rhs) {
    lhs += rhs;
    return lhs;
}

inline Vector3 &operator-=(Vector3 &lhs, const Vector3 &rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    return lhs;
}
inline Vector3 operator-(Vector3 lhs, const Vector3 &rhs) {
    lhs -= rhs;
    return lhs;
}

inline Vector3 &operator*=(Vector3 &lhs, const float &rhs) {
    lhs.x *= rhs;
    lhs.y *= rhs;
    lhs.z *= rhs;
    return lhs;
}
inline Vector3 operator*(Vector3 lhs, const float &rhs) {
    lhs *= rhs;
    return lhs;
}

inline Vector3 &operator/=(Vector3 &lhs, const float &rhs) {
    lhs.x /= rhs;
    lhs.y /= rhs;
    lhs.z /= rhs;
    return lhs;
}
inline Vector3 operator/(Vector3 lhs, const float &rhs) {
    lhs /= rhs;
    return lhs;
}


// Vector4 operator overload
inline std::ostream& operator<<(std::ostream& os, const Vector4& vec) {
    os << '<' << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << '>';
    return os;
}

inline bool operator==(const Vector4 &lhs, const Vector4 &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}
inline bool operator!=(const Vector4 &lhs, const Vector4 &rhs) {
    return !(lhs == rhs);
}

inline Vector4 &operator+=(Vector4 &lhs, const Vector4 &rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    lhs.w += rhs.w;
    return lhs;
}
inline Vector4 operator+(Vector4 lhs, const Vector4 &rhs) {
    lhs += rhs;
    return lhs;
}

inline Vector4 &operator-=(Vector4 &lhs, const Vector4 &rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    lhs.w -= rhs.w;
    return lhs;
}
inline Vector4 operator-(Vector4 lhs, const Vector4 &rhs) {
    lhs -= rhs;
    return lhs;
}

inline Vector4 &operator*=(Vector4 &lhs, const float &rhs) {
    lhs.x *= rhs;
    lhs.y *= rhs;
    lhs.z *= rhs;
    lhs.w *= rhs;
    return lhs;
}
inline Vector4 operator*(Vector4 lhs, const float &rhs) {
    lhs *= rhs;
    return lhs;
}

inline Vector4 &operator/=(Vector4 &lhs, const float &rhs) {
    lhs.x /= rhs;
    lhs.y /= rhs;
    lhs.z /= rhs;
    lhs.w /= rhs;
    return lhs;
}
inline Vector4 operator/(Vector4 lhs, const float &rhs) {
    lhs /= rhs;
    return lhs;
}

#endif