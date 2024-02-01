#ifndef UTIL_COLOR_H
#define UTIL_COLOR_H

#include <iostream>
#include <format>
#include "stdint.h"

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
            a{ static_cast<uint8_t>(color & 0xFF) } {
        rgba_repr = (r << 24) | (g << 16) | (b << 8) | a;
        abgr_repr = (a << 24) | (b << 16) | (g << 8) | r;
    }
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

    uint32_t as_RGBA() const { return rgba_repr; }
    uint32_t as_ABGR() const { return abgr_repr; }
private:
    uint32_t rgba_repr;
    uint32_t abgr_repr;
};

inline std::ostream& operator<<(std::ostream& os, const RGBA& color) {
    os <<  std::format("#{:x} {:x} {:x} {:x}", color.r, color.g, color.b, color.a);
    return os;
}

#endif