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

    void onMouseClick(int localx, int localy, unsigned button) override {
        std::cout << text << "\n";
    };
    void onMouseEnter(int localx, int localy) override {
        std::cout << "entered " << localx << " " << localy << "\n";
    }
    void onMouseLeave(int localx, int localy) override {
        std::cout << "left " << localx << " " << localy << "\n";
    }
private:
    BrushRenderer * brush_renderer;
};

#endif