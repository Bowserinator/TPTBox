#include "SimUI.h"
#include "../../simulation/SimulationDef.h"
#include "../../simulation/ElementClasses.h"
#include "../../simulation/MenuCategories.h"
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
            .backgroundColor = Color{0, 0, 0, 0},
            .textColor = Color{255, 255, 255, 0}
        }
    );
    mainPanel->addChild(elementDescLabel);
    addChild(mainPanel);


    // Bottom setting buttons
    auto getBottomIconButton = [](int slot, guiIconName icon) {
        return new ui::IconButton(
            Vector2{ GetScreenWidth() - slot * styles::SETTINGS_BUTTON_HEIGHT, GetScreenHeight() - styles::SETTINGS_BUTTON_HEIGHT },
            Vector2{styles::SETTINGS_BUTTON_HEIGHT, styles::SETTINGS_BUTTON_HEIGHT},
            icon);
    };

    addChild(getBottomIconButton(2, ICON_IMAGE_SETTINGS)->setClickCallback([this]() {
        addChild(new GraphicsSettingsModal(Vector2{50, 50}, Vector2{500, 500}, renderer));
    }));
    addChild(getBottomIconButton(4, ICON_FILE)->setClickCallback([this]() {
        sim->reset();
    }));

    pauseButton = getBottomIconButton(1, ICON_PLAYER_PAUSE);
    addChild(pauseButton->setClickCallback([this]() { sim->paused = !sim->paused; }));

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
        "Test",
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
}

void SimUI::switchCategory(const MenuCategory category) {
    // Element buttons
    // ------------------
    mainPanel->clearChildren();
    elementButtons.clear();

    int i = 1;
    for (auto id = 1; id <= ELEMENT_COUNT; id++) {
        const auto &el = GetElements()[id];
        if (!el.Enabled || el.MenuSection != category) continue;

        Color bg_color = el.Color.as_Color();
        bg_color.a = 255;

        float btnX = mainPanel->size.x - (i % 22) * (styles::ELEMENT_BUTTON_SIZE.x + 5) - SIDE_PANEL_WIDTH - 5.0f;
        float btnY = (i > 22 ? styles::ELEMENT_BUTTON_SIZE.y + 5 : 0) + 10;

        ui::TextButton * btn = new ui::TextButton(
            Vector2{btnX, btnY},
            styles::ELEMENT_BUTTON_SIZE,
            el.Name,
            (ui::Style {
                .borderColor = bg_color,
                .hoverBorderColor = RED,
                .focusBorderColor = RED,
                .disabledBorderColor = bg_color,
                .borderThickness = 2.0f
            })
                .setAllBackgroundColors(bg_color)
                .setAllTextColors(el.Color.brightness() < 128 ? WHITE : BLACK)
        );
        btn->setClickCallback([this, id]() { brushRenderer->set_selected_element(id); });
        btn->setEnterCallback([this, id]() {
            const auto &el = GetElements()[id];
            elementDescLabel->setText(el.Description);
            elementDescAlpha = 1.0f;
        });
        elementButtons.push_back(btn);
        mainPanel->addChild(btn);
        i++;
    }

    // Side bar
    // -------------------
    auto categoryBtn = categoryButtons[(int)category];
    categoryBtn->focus();

    menuTooltipAlpha = 1.0f;
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

    pauseButton->setIcon(sim->paused ? ICON_PLAYER_PLAY : ICON_PLAYER_PAUSE);
    pauseButton->style.setAllBackgroundColors(sim->paused ? WHITE : BLACK);
    pauseButton->style.setAllTextColors(!sim->paused ? WHITE : BLACK);

    if (elementDescAlpha > 0)
        elementDescAlpha -= 0.6f * GetFrameTime();
    if (menuTooltipAlpha > 0)
        menuTooltipAlpha -= 0.6f * GetFrameTime();

    for (auto child : elementButtons)
        if (child->contains(GetMousePosition() - mainPanel->pos - child->pos)) {
            elementDescAlpha = 1.0f;
            break;
        }
    elementDescAlpha = std::max(0.0f, elementDescAlpha);
    elementDescLabel->style.backgroundColor.a = 128 * elementDescAlpha;
    elementDescLabel->style.textColor.a = 255 * elementDescAlpha;

    menuTooltipAlpha = std::max(0.0f, menuTooltipAlpha);
    menuTooltip->style.backgroundColor.a = 128 * menuTooltipAlpha;
    menuTooltip->style.textColor.a = 255 * menuTooltipAlpha;
}
