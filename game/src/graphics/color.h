#ifndef UTIL_COLOR_H
#define UTIL_COLOR_H

#include <iostream>
#include <format>
#include "stdint.h"
#include "raylib.h"

class RGBA {
public:
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    RGBA(const uint32_t color = 0xFFFFFFF):
            r{ static_cast<uint8_t>((color & 0xFF000000) >> 24) },
            g{ static_cast<uint8_t>((color & 0xFF0000) >> 16) },
            b{ static_cast<uint8_t>((color & 0xFF00) >> 8) },
            a{ static_cast<uint8_t>(color & 0xFF) } {}
    RGBA(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 0xFF):
        r(r), g(g), b(b), a(a) {}

    RGBA(const RGBA &other) = default;
    RGBA(RGBA && other) = default;
    RGBA &operator=(const RGBA &other) = default;
    RGBA &operator=(RGBA &&other) = default;

    void darken(float amt) {
        r *= amt;
        g *= amt;
        b *= amt;
    }

    uint8_t brightness() const { // Yes it's not very accurate...
        return static_cast<uint8_t>(0.2 * r + 0.7 * g + 0.1 * b);
    }

    // Return copy with specified alpha
    RGBA withAlpha(const float alpha) const {
        return RGBA(r, g, b, alpha);
    }

    // Blend self with other color (alpha blending). Other is on top of current
    RGBA blend(const RGBA &other) const {
        const float a_out = other.a + a * (1 - other.a);
        return RGBA(
            static_cast<uint8_t>((other.r * other.a + r * a * (1 - other.a)) / a_out),
            static_cast<uint8_t>((other.g * other.a + g * a * (1 - other.a)) / a_out),
            static_cast<uint8_t>((other.b * other.a + b * a * (1 - other.a)) / a_out),
            a_out
        );
    }

    uint32_t as_RGBA() const { return (r << 24) | (g << 16) | (b << 8) | a; }
    uint32_t as_ABGR() const { return (a << 24) | (b << 16) | (g << 8) | r; }
    Color as_Color() const { return Color{r, g, b, a}; }
};

inline std::ostream& operator<<(std::ostream& os, const RGBA& color) {
    os <<  std::format("#{:x} {:x} {:x} {:x}", color.r, color.g, color.b, color.a);
    return os;
}

#endif