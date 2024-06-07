#ifndef INTERFACE_GUI_COMPONENTS_CHECKBOX_H_
#define INTERFACE_GUI_COMPONENTS_CHECKBOX_H_

#include "raylib.h"
#include "../styles.h"
#include "../Style.h"
#include "./abstract/InteractiveComponent.h"
#include "../../FontCache.h"

#include <functional>
#include <string>

namespace ui {
    class Checkbox: public InteractiveComponent {
    public:
        Checkbox(
            const Vector2 &pos,
            const Style &style = Style::getDefault(),
            const Vector2 &size = Vector2{styles::CHECKBOX_SIZE, styles::CHECKBOX_SIZE}
        ): InteractiveComponent(pos, size, style, MOUSE_CURSOR_POINTING_HAND) {}

        void draw(const Vector2 &screenPos) override {
            constexpr float PAD = 5.0f;
            DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, styles::CHECKBOX_SIZE, styles::CHECKBOX_SIZE },
                style.borderThickness,
                style.getBorderColor(this));
            if (m_checked)
                DrawRectangle(
                    screenPos.x + PAD, screenPos.y + PAD,
                    styles::CHECKBOX_SIZE - 2 * PAD, styles::CHECKBOX_SIZE - 2 * PAD,
                    style.getTextColor(this));
        }

        void onMouseClick(Vector2 localPos, unsigned button) override {
            InteractiveComponent::onMouseClick(localPos, button);
            if (!m_disabled) {
                toggle();
                m_click_callback(m_checked);
            }
        }

        bool checked() const { return m_checked; }
        void setChecked(bool checked) { m_checked = checked; }
        void toggle() { m_checked = !m_checked; }

        Checkbox * setClickCallback(const std::function<void (bool)> &f) { m_click_callback = f; return this; }

    protected:
        bool m_checked = false;
        std::function<void (bool)> m_click_callback = [](bool){};
    };


    // Checkbox with a label
    class LabeledCheckbox: public Checkbox {
    public:
        LabeledCheckbox(
            const Vector2 &pos,
            const Vector2 &size,
            const std::string &text,
            const Style &style = Style {
                .horizontalAlign = Style::Align::Left
            }
        ): Checkbox(pos, style, size), m_text(text) {
            m_tsize = MeasureTextEx(FontCache::ref()->main_font, text.c_str(), FONT_SIZE, FONT_SPACING);
        }

        void setText(const std::string &str) {
            m_text = str;
            m_tsize = MeasureTextEx(FontCache::ref()->main_font, m_text.c_str(), FONT_SIZE, FONT_SPACING);
        }

        void draw(const Vector2 &screenPos) override {
            Checkbox::draw(screenPos);

            const Vector2 pad = style.align(size, Vector2{ 5, 5 }, m_tsize);
            SetTextLineSpacing(FONT_SIZE);
            DrawTextEx(FontCache::ref()->main_font, m_text.c_str(),
                Vector2{screenPos.x + styles::CHECKBOX_SIZE + 8 + pad.x, screenPos.y + pad.y},
                FONT_SIZE, FONT_SPACING,
                style.getTextColor(this));
        }

    protected:
        Vector2 m_tsize;
        std::string m_text;
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_CHECKBOX_H_
