#include "gradient.h"
#include "../util/math.h"

namespace graphics {
    bool GradientStop::operator <(const GradientStop &other) const {
        return point < other.point;
    }

    Gradient generate_gradient_table(std::vector<GradientStop> stops, int resolution) {
        Gradient table(resolution, 0xFF);
        if (stops.size() >= 2) {
            std::sort(stops.begin(), stops.end());

            auto stop = -1;
            for (auto i = 0; i < resolution; ++i) {
                auto point = i / (float)resolution;
                while (stop < (int)stops.size() - 1 && stops[stop + 1].point <= point)
                    ++stop;
                if (stop < 0 || stop >= (int)stops.size() - 1)
                    continue;

                auto &left = stops[stop];
                auto &right = stops[stop + 1];
                auto f = (point - left.point) / (right.point - left.point);
                table[i] = left.color.blend(right.color.withAlpha(uint8_t(f * 0xFF)));
            }
        }
        return table;
    }

    RGBA gradient_get(const Gradient &grad, float amt) {
        amt = util::clampf(amt, 0.0f, 1.0f);
        return grad[static_cast<int>(amt * (grad.size() - 1))];
    }

    namespace gradients {
        const Gradient heat_gradient = generate_gradient_table({
                { 0x2B00FFFF, 0.00f },
                { 0x003CFFFF, 0.01f },
                { 0x00C0FFFF, 0.05f },
                { 0x00FFEBFF, 0.08f },
                { 0x00FF14FF, 0.19f },
                { 0x4BFF00FF, 0.25f },
                { 0xC8FF00FF, 0.37f },
                { 0xFFDC00FF, 0.45f },
                { 0xFF0000FF, 0.71f },
                { 0xFF00DCFF, 1.00f },
            }, 1024);
    }
}
