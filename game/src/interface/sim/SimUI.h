#ifndef SIM_UI_H
#define SIM_UI_H

#include "../gui/Scene.h"
#include "../gui/components/Label.h"
#include "../gui/components/TextButton.h"
#include "../gui/components/IconButton.h"
#include "../gui/components/Panel.h"
#include <vector>

class BrushRenderer;
class Renderer;
class Simulation;

class SimUI : public ui::Scene {
public:
    SimUI(BrushRenderer * brushRenderer, Renderer * renderer, Simulation * sim);

    void init() override;
    void update() override;
private:
    BrushRenderer * brushRenderer;
    Renderer * renderer;
    Simulation * sim;

    ui::Label * elementDescLabel = nullptr;
    ui::Panel * mainPanel = nullptr;
    ui::IconButton * pauseButton = nullptr;

    float elementDescAlpha = 0.0f;
    std::vector<ui::TextButton *> elementButtons;
};

#endif