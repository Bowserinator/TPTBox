#ifndef GRAPHICS_GRADIENT_H_
#define GRAPHICS_GRADIENT_H_

#include <vector>
#include <algorithm>

#include "color.h"

namespace graphics {
    using Gradient = std::vector<RGBA>;

    struct GradientStop{
        RGBA color;
        float point;

        bool operator <(const GradientStop &other) const;
    };

    Gradient generate_gradient_table(std::vector<GradientStop> stops, int resolution);
    RGBA gradient_get(const Gradient &grad, float amt);

    namespace gradients {
        extern const Gradient heat_gradient;
    }
} // namespace graphics

#endif // GRAPHICS_GRADIENT_H_
