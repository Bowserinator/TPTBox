#ifndef INTERFACE_GUI_COMPONENTS_SLIDER_H_
#define INTERFACE_GUI_COMPONENTS_SLIDER_H_

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

        float percent() const { return m_percent; }
        void setPercent(float val) { m_percent = val; }

        Slider * setChangeCallback(const std::function<void (float)> &f) { m_change_callback = f; return this; }
    protected:
        std::function<void (float)> m_change_callback = [](float){};
        bool m_dragging = false;
        float m_percent = 0.0;

        void _limitPercent() { m_percent = util::clamp(m_percent, 0.0f, 1.0f); }
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_SLIDER_H_
