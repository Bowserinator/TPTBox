#include "SimUI.h"
#include "../../simulation/SimulationDef.h"
#include "../../simulation/ElementClasses.h"
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

SimUI::SimUI(BrushRenderer * brushRenderer, Renderer * renderer, Simulation * sim):
    brushRenderer(brushRenderer), renderer(renderer), sim(sim) {}

void SimUI::init() {
    mainPanel = new ui::Panel(
        Vector2{ 0, (float)GetScreenHeight() - 130 },
        Vector2{ (float)GetScreenWidth(), 130 }
    );

    // Label for displaying element descriptions
    elementDescLabel = new ui::Label(
        Vector2{ (float)GetScreenWidth() - 1005, 0 },
        Vector2{ 1000.0f, 30.0f },
        "",
        ui::Style {
            .horizontalAlign = ui::Style::Align::Right,
            .backgroundColor = Color{0, 0, 0, 0},
            .textColor = Color{255, 255, 255, 0}
        }
    );
    mainPanel->addChild(elementDescLabel);

    // Element buttons
    for (auto id = 1; id <= ELEMENT_COUNT; id++) {
        const auto &el = GetElements()[id];
        Color bg_color = el.Color.as_Color();
        bg_color.a = 255;

        float btnX = mainPanel->size.x - (id % 22) * (styles::ELEMENT_BUTTON_SIZE.x + 5);
        float btnY = (id > 22 ? styles::ELEMENT_BUTTON_SIZE.y + 5 : 0) + 35;

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
        btn->setEnterCallback([this, &el]() {
            elementDescLabel->setText(el.Description);
            elementDescAlpha = 1.0f;
        });
        elementButtons.push_back(btn);
        mainPanel->addChild(btn);
    }

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
    pauseButton = getBottomIconButton(1, ICON_PLAYER_PAUSE);
    addChild(pauseButton->setClickCallback([this]() { sim->paused = !sim->paused; }));
}

void SimUI::update() {
    Scene::update();

    pauseButton->setIcon(sim->paused ? ICON_PLAYER_PLAY : ICON_PLAYER_PAUSE);
    pauseButton->style.setAllBackgroundColors(sim->paused ? WHITE : BLACK);
    pauseButton->style.setAllTextColors(!sim->paused ? WHITE : BLACK);

    if (elementDescAlpha > 0)
        elementDescAlpha -= 0.01f;
    for (auto child : elementButtons)
        if (child->contains(GetMousePosition() - mainPanel->pos - child->pos)) {
            elementDescAlpha = 1.0f;
            break;
        }
    elementDescAlpha = std::max(0.0f, elementDescAlpha);
    elementDescLabel->style.backgroundColor.a = 128 * elementDescAlpha;
    elementDescLabel->style.textColor.a = 255 * elementDescAlpha;
}
