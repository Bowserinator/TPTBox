#ifndef ELEMENT_MENU_H
#define ELEMENT_MENU_H

#include "../gui/components/Panel.h"
#include "../gui/Scene.h"

class BrushRenderer;

class ElementMenu : public ui::Scene {
public:
    ElementMenu(BrushRenderer * brush_renderer);

    void init() override;
private:
    BrushRenderer * brush_renderer;
};

#endif