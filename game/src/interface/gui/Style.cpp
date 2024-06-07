#include "Style.h"
#include "./components/abstract/Component.h"
#include "../../util/vector_op.h"

using namespace ui;

Color Style::getBackgroundColor(Component * c) const {
    if (c->disabled()) return disabledBackgroundColor;
    if (c->hovered()) return hoverBackgroundColor;
    if (c->focused()) return focusBackgroundColor;
    return backgroundColor;
}

Color Style::getTextColor(Component * c) const {
    if (c->disabled()) return disabledTextColor;
    if (c->hovered()) return hoverTextColor;
    if (c->focused()) return focusTextColor;
    return textColor;
}

Color Style::getBorderColor(Component * c) const {
    if (c->disabled()) return disabledBorderColor;
    if (c->hovered()) return hoverBorderColor;
    if (c->focused()) return focusBorderColor;
    return borderColor;
}

Vector2 Style::align(const Vector2 &size, Vector2 pad, const Vector2 &innerSize) const {
    const Vector2 padCenter = (size - innerSize) / 2.0f;
    if (horizontalAlign == Align::Center)
        pad.x = padCenter.x;
    else if (horizontalAlign == Align::Right)
        pad.x = size.x - pad.x - innerSize.x;

    if (verticalAlign == Align::Center)
        pad.y = padCenter.y;
    else if (verticalAlign == Align::Right)
        pad.y = size.y - pad.y - innerSize.y;

    return pad;
}
