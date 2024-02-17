#include "ElementMenu.h"
#include "../../simulation/SimulationDef.h"
#include "../../simulation/ElementClasses.h"
#include "../brush/Brush.h"
#include "../../render/Renderer.h"

#include "../gui/components/Panel.h"
#include "../gui/components/Modal.h"
#include "../gui/components/TextButton.h"
#include "../gui/components/Checkbox.h"
#include "../gui/components/Label.h"
#include "../gui/styles.h"

#include "../settings/GraphicsSettingsModal.h"

ElementMenu::ElementMenu(BrushRenderer * brushRenderer, Renderer * renderer):
    brushRenderer(brushRenderer), renderer(renderer) {}

void ElementMenu::init() {
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

    addChild(new GraphicsSettingsModal(Vector2{50, 50}, Vector2{500, 500}, renderer));
}

void ElementMenu::update() {
    Scene::update();

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
