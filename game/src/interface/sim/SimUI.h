#ifndef INTERFACE_SIM_SIMUI_H_
#define INTERFACE_SIM_SIMUI_H_

#include "../../simulation/MenuCategories.h"
#include "../gui/Scene.h"
#include "../gui/components/Label.h"
#include "../gui/components/TextButton.h"
#include "../gui/components/IconButton.h"
#include "../gui/components/Panel.h"
#include "ConsolePanel.h"

#include <map>
#include <vector>

class BrushRenderer;
class Renderer;
class Simulation;

class BrushViewModal;
class BrushShapeToolModal;

class SimUI : public ui::Scene {
public:
    SimUI(BrushRenderer * brushRenderer, Renderer * renderer, Simulation * sim);

    void init() override;
    void update() override;

private:
    void switchCategory(const MenuCategory category);
    void showConsole();
    void hideConsole();

    BrushRenderer * brushRenderer;
    Renderer * renderer;
    Simulation * sim;

    BrushViewModal * brushViewWindow = nullptr;
    BrushShapeToolModal * brushShapeToolWindow = nullptr;

    bool sim_paused_before = false;
    bool console_active = false;
    ConsolePanel * consolePanel;

    ui::Label * elementDescLabel = nullptr;
    ui::Label * menuTooltip = nullptr;
    ui::Label * bottomTooltip = nullptr;

    ui::Panel * mainPanel = nullptr;
    ui::Panel * sidePanel = nullptr;
    ui::IconButton * pauseButton = nullptr;

    enum TOOLTIP_ALPHAS { ELEMENT_DESC_TOOLTIP = 0, MENU_TOOLTIP, BOTTOM_TOOLTIP };
    std::array<float, 3> tooltipAlphas = { 0.0f, 0.0f, 0.0f };
    std::array<ui::Label *, 3> tooltips;

    std::vector<ui::TextButton *> elementButtons;
    std::vector<ui::IconButton *> categoryButtons;
    std::map<int, ui::IconButton *> bottomBarButtons;
};

#endif // INTERFACE_SIM_SIMUI_H_
