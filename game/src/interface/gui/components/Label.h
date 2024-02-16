#ifndef GUI_LABEL_H
#define GUI_LABEL_H

#include "raylib.h"
#include "../styles.h"
#include "../Style.h"
#include "../../FontCache.h"
#include "../../../util/vector_op.h"
#include "./abstract/Component.h"

#include <string>

namespace ui {
    class Label: public Component {
    public:
        Label(
            const Vector2 &pos, 
            const Vector2 &size,
            const std::string &text,
            const Style &style = (Style {
                    .horizontalAlign = Style::Align::Left
                }).setAllBackgroundColors(Color{0, 0, 0, 0})
        ): Component(pos, size, style), text(text) {
            tsize = MeasureTextEx(FontCache::ref()->main_font, text.c_str(), FONT_SIZE, FONT_SPACING);
        }

        void setText(const std::string &str) {
            text = str;
            tsize = MeasureTextEx(FontCache::ref()->main_font, text.c_str(), FONT_SIZE, FONT_SPACING);
        }

        void draw(const Vector2 &screenPos) override {
            const Vector2 pad = style.align(size, Vector2{ 5, 5 }, tsize);
            if (style.backgroundColor.a)
                DrawRectangle(screenPos.x + pad.x - 5, screenPos.y, tsize.x + 10, size.y, style.getBackgroundColor(this));
            
            SetTextLineSpacing(FONT_SIZE);
            DrawTextEx(FontCache::ref()->main_font, text.c_str(), screenPos + pad, FONT_SIZE, FONT_SPACING,
                style.getTextColor(this));
        }
    
    protected:
        Vector2 tsize;
        std::string text;
    };
}

#endif