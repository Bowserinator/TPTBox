#ifndef ELEMENT_MENU_H
#define ELEMENT_MENU_H

#include "../gui/Scene.h"
#include "../gui/components/Label.h"
#include "../gui/components/TextButton.h"
#include "../gui/components/Panel.h"
#include <vector>

class BrushRenderer;

class ElementMenu : public ui::Scene {
public:
    ElementMenu(BrushRenderer * brushRenderer);

    void init() override;
    void update() override;
private:
    BrushRenderer * brushRenderer;
    ui::Label * elementDescLabel = nullptr;
    ui::Panel * mainPanel = nullptr;

    float elementDescAlpha = 0.0f;
    std::vector<ui::TextButton *> elementButtons;
};

#endif