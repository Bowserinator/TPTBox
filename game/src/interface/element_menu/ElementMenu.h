#ifndef ELEMENT_MENU_H
#define ELEMENT_MENU_H

#include "../gui/components/Panel.h"

class BrushRenderer;

class ElementMenu {
public:
    ElementMenu(BrushRenderer * brush_renderer);

    void init();
    void draw();
    void update();
private:
    ui::Panel main_panel;
    BrushRenderer * brush_renderer;
};

#endif