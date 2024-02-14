#ifndef GUI_STYLE_H
#define GUI_STYLE_H

#include "raylib.h"
#include "styles.h"

namespace ui {
    class Component;
    class Style {
    public:
        enum class Align { Left = 0, Top = 0, Center = 1, Right = 2, Bottom = 2 };

        Align horizontalAlign = Align::Center;
        Align verticalAlign = Align::Center;

        Color backgroundColor         = BLACK;
        Color hoverBackgroundColor    = BLACK;
        Color focusBackgroundColor    = BLACK;
        Color disabledBackgroundColor = BLACK;

        Color hoverTextColor    = WHITE;
        Color textColor         = WHITE;
        Color focusTextColor    = WHITE;
        Color disabledTextColor = Color{ 120, 120, 120, 255 };

        Color borderColor         = Color{ 120, 120, 120, 255 };
        Color hoverBorderColor    = WHITE;
        Color focusBorderColor    = Color{ 120, 120, 120, 255 };
        Color disabledBorderColor = Color{ 120, 120, 120, 255 };

        float borderThickness = 1.0f;

        // Compute alignment of an object inside another with current alignments
        // @param size Size of outer bounding box
        // @param pad Padding of outer bounding box
        // @param innerSize size of inner object to align
        // @return Offset from top left of outer bounding box
        Vector2 align(const Vector2 &size, Vector2 pad, const Vector2 &innerSize) const;

        Color getBackgroundColor(Component * c) const;
        Color getTextColor(Component * c) const;
        Color getBorderColor(Component * c) const;

        Style& setAllBackgroundColors(const Color &c) {
            backgroundColor = c;
            hoverBackgroundColor = c;
            focusBackgroundColor = c;
            disabledBackgroundColor = c;
            return *this;
        }

        Style& setAllTextColors(const Color &c) {
            hoverTextColor = c;
            textColor = c;
            focusTextColor = c;
            disabledTextColor = c;
            return *this;
        }

        Style &setAllBorderColors(const Color &c) {
            borderColor = c;
            hoverBorderColor = c;
            focusBorderColor = c;
            disabledBorderColor = c;
            return *this;
        }

        static Style getDefault() {
            Style style;
            return style;
        }
    };
}

#endif