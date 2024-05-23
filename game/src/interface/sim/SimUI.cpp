#include "SimUI.h"
#include "../../simulation/SimulationDef.h"
#include "../../simulation/ElementClasses.h"
#include "../../simulation/ToolClasses.h"
#include "../../simulation/MenuCategories.h"
#include "../../simulation/Gol.h"
#include "../brush/Brush.h"
#include "../../render/Renderer.h"

#include "../gui/components/Panel.h"
#include "../gui/components/Modal.h"
#include "../gui/components/TextButton.h"
#include "../gui/components/IconButton.h"
#include "../gui/components/Checkbox.h"
#include "../gui/components/Label.h"
#include "../gui/styles.h"

#include "../settings/GraphicsSettingsModal.h"
#include "../settings/SimSettingsModal.h"
#include "../settings/ConfirmExitModal.h"

const float SIDE_PANEL_WIDTH = styles::SETTINGS_BUTTON_HEIGHT;
constexpr float MAIN_PANEL_HEIGHT = 90;

SimUI::SimUI(BrushRenderer * brushRenderer, Renderer * renderer, Simulation * sim):
    brushRenderer(brushRenderer), renderer(renderer), sim(sim) {}

void SimUI::init() {
    mainPanel = new ui::Panel(
        Vector2{ 0, (float)GetScreenHeight() - MAIN_PANEL_HEIGHT },
        Vector2{ (float)GetScreenWidth(), MAIN_PANEL_HEIGHT },
        ui::Style { .backgroundColor = BLACK }
    );

    // Label for displaying element descriptions
    elementDescLabel = new ui::Label(
        Vector2{ (float)GetScreenWidth() - 975 - SIDE_PANEL_WIDTH, -30.0f },
        Vector2{ 1000.0f - SIDE_PANEL_WIDTH, 30.0f },
        "",
        ui::Style {
            .horizontalAlign = ui::Style::Align::Right,
            .backgroundColor = BLANK,
            .textColor = Color{255, 255, 255, 0}
        }
    );
    mainPanel->addChild(elementDescLabel);
    addChild(mainPanel);


    // Bottom setting buttons
    auto getBottomIconButton = [this](int slot, guiIconName icon, const std::string &tooltip) {
        auto btn = new ui::IconButton(
            Vector2{ GetScreenWidth() - slot * styles::SETTINGS_BUTTON_HEIGHT, GetScreenHeight() - styles::SETTINGS_BUTTON_HEIGHT },
            Vector2{styles::SETTINGS_BUTTON_HEIGHT, styles::SETTINGS_BUTTON_HEIGHT},
            icon);
        btn->setEnterCallback([this, btn, tooltip]() {
            bottomTooltip->pos.x = btn->pos.x - 700.0f + styles::SETTINGS_BUTTON_HEIGHT;
            bottomTooltip->setText(tooltip);
            tooltipAlphas[BOTTOM_TOOLTIP] = 1.0f;
        });
        bottomBarButtons[slot] = btn;
        return btn;
    };

    addChild(getBottomIconButton(2, ICON_IMAGE_SETTINGS, "Graphics Settings")->setClickCallback([this]() {
        addChild(new GraphicsSettingsModal(Vector2{(float)GetScreenWidth() / 2 - 250, (float)GetScreenHeight() / 2 - 300},
        Vector2{500, 500}, renderer));
    }));
    addChild(getBottomIconButton(3, ICON_SIM_SETTINGS, "Sim Settings")->setClickCallback([this]() {
        addChild(new SimSettingsModal(Vector2{(float)GetScreenWidth() / 2 - 250, (float)GetScreenHeight() / 2 - 300},
        Vector2{500, 500}, sim));
    }));
    addChild(getBottomIconButton(4, ICON_FILE, "Clear Sim")->setClickCallback([this]() {
        sim->reset();
    }));

    pauseButton = getBottomIconButton(1, ICON_PLAYER_PAUSE, "");
    addChild(pauseButton->setClickCallback([this]() { sim->paused = !sim->paused; }));

    // Tooltip for bottom button
    bottomTooltip = new ui::Label(
        Vector2{ 0.0f, GetScreenHeight() - styles::SETTINGS_BUTTON_HEIGHT * 2 - 5.0f },
        Vector2{ 700.0f, styles::SETTINGS_BUTTON_HEIGHT },
        "",
        ui::Style {
            .horizontalAlign = ui::Style::Align::Right,
            .backgroundColor = BLANK,
            .textColor = Color{255, 255, 255, 0}
        }
    );
    addChild(bottomTooltip);


    // Side panel
    // -------------------------------
    sidePanel = new ui::Panel(
        Vector2{ (float)GetScreenWidth() - SIDE_PANEL_WIDTH, 0 },
        Vector2{ SIDE_PANEL_WIDTH, (float)GetScreenHeight() - styles::SETTINGS_BUTTON_HEIGHT }
    );

    // Tooltip for menu category
    menuTooltip = new ui::Label(
        Vector2{ -710.0f, 0.0f },
        Vector2{ 700.0f, styles::SETTINGS_BUTTON_HEIGHT },
        "",
        ui::Style {
            .horizontalAlign = ui::Style::Align::Right,
            .backgroundColor = BLANK,
            .textColor = Color{255, 255, 255, 0}
        }
    );
    sidePanel->addChild(menuTooltip);

    addChild(sidePanel);

    for (int i = 0; i < (int)MenuCategory::HIDDEN; i++) {
        ui::IconButton * categoryBtn = new ui::IconButton(
            Vector2{ 0, sidePanel->size.y - ((int)MenuCategory::HIDDEN - i) * styles::SETTINGS_BUTTON_HEIGHT },
            Vector2{styles::SETTINGS_BUTTON_HEIGHT, styles::SETTINGS_BUTTON_HEIGHT},
            CATEGORY_DATA[i].icon);
        categoryBtn->setEnterCallback([this, i]() {
            switchCategory((MenuCategory)i);
        });
        categoryButtons.push_back(categoryBtn);
        sidePanel->addChild(categoryBtn);
    }

    switchCategory((MenuCategory)0);

    tooltips = { elementDescLabel, menuTooltip, bottomTooltip };
}

void SimUI::switchCategory(const MenuCategory category) {
    // Element and tool buttons
    // ------------------
    for (auto btn : elementButtons)
        mainPanel->removeChild(btn);
    elementButtons.clear();

    int i = 1;

    auto getElementButton = [this](const int i, const std::string &name, const RGBA color) {
        Color bg_color = color.as_Color();
        bg_color.a = 255;

        float btnX = mainPanel->size.x - (i % 22) * (styles::ELEMENT_BUTTON_SIZE.x + 5) - SIDE_PANEL_WIDTH - 5.0f;
        float btnY = (i > 22 ? styles::ELEMENT_BUTTON_SIZE.y + 5 : 0) + 10;

        return new ui::TextButton(
            Vector2{btnX, btnY},
            styles::ELEMENT_BUTTON_SIZE,
            name,
            (ui::Style {
                .borderColor = bg_color,
                .hoverBorderColor = RED,
                .focusBorderColor = RED,
                .disabledBorderColor = bg_color,
                .borderThickness = 2.0f
            })
                .setAllBackgroundColors(bg_color)
                .setAllTextColors(color.brightness() < 128 ? WHITE : BLACK)
        );
    };

    // Special: GOL menu
    if (category == MenuCategory::LIFE) {
        for (int j = 0; j < GOL_RULE_COUNT; j++) {
            ui::TextButton * btn = getElementButton(i, golRules[j].name, golRules[j].color);
            btn->setClickCallback([this, j]() {
                brushRenderer->set_selected_tool(TOOL_GOL);
                brushRenderer->set_misc_data(j + 1); // id = index + 1 since 0 is reserved for EMPTY
            });
            btn->setEnterCallback([this, j]() {
                elementDescLabel->setText(golRules[j].description);
                tooltipAlphas[ELEMENT_DESC_TOOLTIP] = 1.0f;
            });
            elementButtons.push_back(btn);
            mainPanel->addChild(btn);
            i++;
        }
    }

    // Add regular tools and elements
    for (auto id = 1; id <= __GLOBAL_TOOL_COUNT; id++) {
        const auto &tool = GetTools()[id];
        if (!tool.Enabled || tool.MenuSection != category) continue;
        
        ui::TextButton * btn = getElementButton(i, tool.Name, tool.Color);
        btn->setClickCallback([this, id]() { brushRenderer->set_selected_tool(id); });
        btn->setEnterCallback([this, id]() {
            const auto &tool = GetTools()[id];
            elementDescLabel->setText(tool.Description);
            tooltipAlphas[ELEMENT_DESC_TOOLTIP] = 1.0f;
        });
        elementButtons.push_back(btn);
        mainPanel->addChild(btn);
        i++;
    }

    for (auto id = 1; id <= ELEMENT_COUNT; id++) {
        const auto &el = GetElements()[id];
        if (!el.Enabled || el.MenuSection != category) continue;

        ui::TextButton * btn = getElementButton(i, el.Name, el.Color);
        btn->setClickCallback([this, id]() { brushRenderer->set_selected_element(id); });
        btn->setEnterCallback([this, id]() {
            const auto &el = GetElements()[id];
            elementDescLabel->setText(el.Description);
            tooltipAlphas[ELEMENT_DESC_TOOLTIP] = 1.0f;
        });
        elementButtons.push_back(btn);
        mainPanel->addChild(btn);
        i++;
    }

    // Side bar
    // -------------------
    auto categoryBtn = categoryButtons[(int)category];
    categoryBtn->focus();

    tooltipAlphas[MENU_TOOLTIP] = 1.0f;
    menuTooltip->setText(CATEGORY_DATA[(int)category].name);
    menuTooltip->pos.y = categoryBtn->pos.y;

    // Reset styles of other buttons
    for (auto btn : categoryButtons) {
        btn->style.setAllBackgroundColors(BLACK);
        btn->style.setAllTextColors(WHITE);
    }

    // Focus style this button
    categoryBtn->style.setAllBackgroundColors(WHITE);
    categoryBtn->style.setAllTextColors(BLACK);
}

void SimUI::update() {
    Scene::update();
    
    // Update positions on resize
    if (IsWindowResized()) {
        mainPanel->pos.x = (float)GetScreenWidth() - mainPanel->size.x;
        mainPanel->pos.y = (float)GetScreenHeight() - MAIN_PANEL_HEIGHT;
        sidePanel->pos.x = (float)GetScreenWidth() - SIDE_PANEL_WIDTH;
        sidePanel->pos.y = (float)GetScreenHeight() - sidePanel->size.y;
        bottomTooltip->pos.y = GetScreenHeight() - styles::SETTINGS_BUTTON_HEIGHT * 2 - 5.0f;

        for (auto [slot, btn] : bottomBarButtons)
            btn->pos = Vector2{
                GetScreenWidth() - slot * styles::SETTINGS_BUTTON_HEIGHT,
                GetScreenHeight() - styles::SETTINGS_BUTTON_HEIGHT
            };
    }

    // ESC to exit game (if no current modal)
    if (EventConsumer::ref()->isKeyPressed(KEY_ESCAPE) || WindowShouldClose()) {
        addChild(new ConfirmExitModal(Vector2{(float)GetScreenWidth() / 2 - 250, (float)GetScreenHeight() / 2 - 50},
            Vector2{500, 130}));
    }

    // Update the rest
    pauseButton->setIcon(sim->paused ? ICON_PLAYER_PLAY : ICON_PLAYER_PAUSE);
    pauseButton->style.setAllBackgroundColors(sim->paused ? WHITE : BLACK);
    pauseButton->style.setAllTextColors(!sim->paused ? WHITE : BLACK);

    for (auto i = 0; i < tooltipAlphas.size(); i++) {
        auto &alpha = tooltipAlphas[i];
        auto tooltip = tooltips[i];

        if (alpha > 0) alpha -= 0.6f * GetFrameTime();
        alpha = std::max(0.0f, alpha);

        tooltip->style.backgroundColor.a = 128 * alpha;
        tooltip->style.textColor.a = 255 * alpha;
    }

    for (auto child : elementButtons)
        if (child->contains(GetMousePosition() - mainPanel->pos - child->pos)) {
            tooltipAlphas[ELEMENT_DESC_TOOLTIP] = 1.0f;
            break;
        }
}
