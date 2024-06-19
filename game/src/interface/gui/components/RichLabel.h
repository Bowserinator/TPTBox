#ifndef INTERFACE_GUI_COMPONENTS_RICHLABEL_H_
#define INTERFACE_GUI_COMPONENTS_RICHLABEL_H_

#include "Label.h"
#include "../../../util/colored_text.h"
#include <string>

namespace ui {
    class RichLabel: public Label {
    public:
        RichLabel(
            const Vector2 &pos,
            const Vector2 &size,
            const std::string &text,
            const Style &style = (Style {
                    .horizontalAlign = Style::Align::Left
                }).setAllBackgroundColors(BLANK)
        ): Label(pos, size, text, style) {}

        void draw(const Vector2 &screenPos) override {
            const Vector2 pad = style.align(size, Vector2{ 5, 5 }, m_tsize);
            if (style.backgroundColor.a)
                DrawRectangle(screenPos.x + pad.x - 5, screenPos.y,
                    m_tsize.x + 10, size.y, style.getBackgroundColor(this));

            SetTextLineSpacing(FONT_SIZE);
            text_format::draw_text_colored_ex(FontCache::ref()->main_font, m_text,
                screenPos + pad, FONT_SIZE, FONT_SPACING,
                style.getTextColor(this));
        }
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_RICHLABEL_H_
