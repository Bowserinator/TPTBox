#ifndef GUI_SLIDER_H
#define GUI_SLIDER_H

#include "raylib.h"
#include "./abstract/InteractiveComponent.h"
#include "../styles.h"
#include "../Style.h"
#include "../../../util/math.h"

#include <functional>


namespace ui {
    class Slider: public InteractiveComponent {
    public:
        Slider(
            const Vector2 &pos,
            const Vector2 &size,
            const Style &style = Style { .backgroundColor = styles::SLIDER_DEFAULT_BG_COLOR }
        ): InteractiveComponent(pos, size, style) {}

        void draw(const Vector2 &screenPos) override;

        void onMouseMoved(Vector2 localPos) override;
		void onMouseClick(Vector2 localPos, unsigned button) override;
        void onMouseRelease(Vector2 localPos, unsigned button) override;

        float getPercent() const { return percent; }
        void setPercent(float val) { percent = val; }

        Slider * setChangeCallback(const std::function<void (float)> &f) { changeCallback = f; return this; }
    protected:
        std::function<void (float)> changeCallback = [](float){};
        bool dragging = false;
        float percent = 0.0;

        void _limitPercent() { percent = util::clamp(percent, 0.0f, 1.0f); }
    };
}

#endif