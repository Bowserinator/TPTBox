#ifndef ELEMENT_MENU_BUTTON_H
#define ELEMENT_MENU_BUTTON_H

#include "../gui/components/TextButton.h"
#include <iostream>

constexpr Vector2 ELEMENT_BUTTON_SIZE{ 60, 32 };

class BrushRenderer;

class ElementMenuButton : public ui::TextButton {
public:
    ElementMenuButton(
        const Vector2 &pos,
        const std::string label,
        Color bg_color,
        Color color,
        BrushRenderer * brush_renderer
    ):
        ui::TextButton(pos, ELEMENT_BUTTON_SIZE, label, bg_color, color, bg_color),
        brush_renderer(brush_renderer) {};

    virtual ~ElementMenuButton() = default;

    void onMouseClick(Vector2 localPos, unsigned button) override {
        TextButton::onMouseClick(localPos, button);
        std::cout << "click: " << text << "\n";
    };
    void onMouseEnter(Vector2 localPos) override {
        TextButton::onMouseEnter(localPos);
        std::cout << "entered " << text << localPos.x << " " << localPos.y << "\n";
    }
    void onMouseLeave(Vector2 localPos) override {
        TextButton::onMouseLeave(localPos);
        std::cout << "left " << text << localPos.x << " " << localPos.y << "\n";
    }
private:
    BrushRenderer * brush_renderer;
};

#endif