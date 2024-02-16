#include "GraphicsSettingsModal.h"
#include "../gui/components/TextButton.h"
#include "../gui/components/Checkbox.h"
#include "../gui/components/Dropdown.h"
#include "../gui/components/Label.h"
#include "../gui/components/HR.h"
#include "../gui/styles.h"

using namespace ui;

GraphicsSettingsModal::GraphicsSettingsModal(const Vector2 &pos, const Vector2 &size):
    ui::Window(pos, size, "Graphics Settings", Style::getDefault())
{
    addChild(new TextButton(
        Vector2{ 0, size.y - styles::SETTINGS_BUTTON_HEIGHT },
        Vector2{ size.x, styles::SETTINGS_BUTTON_HEIGHT },
        "OK"
    ));

    panel->addChild(new Label(
        Vector2{ 20.0f, 0.0f },
        Vector2{ size.x, 20.0f },
        "Settings will apply once you hit \"OK\"",
        Style {
            .horizontalAlign = Style::Align::Left,
            .textColor = YELLOW
        }
    ));

    constexpr float spacing = 26.0f;
    float Y = 30.0f;
    panel->addChild((new Dropdown(
            Vector2{ size.x - styles::DROPDOWN_SIZE.x - 20.0f, Y },
            Vector2{ styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y })
        )
        ->addOption("Normal renderer", 0)
        ->addOption("Debug: step count", 1)
        ->addOption("Debug: normal vectors", 2)
        ->addOption("Debug: ambient occlusion", 3)
        ->switchToOption(0));
    panel->addChild(new Label(
        Vector2{ 20.0f, Y },
        Vector2{ size.x - styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y },
        "Renderer mode"
    ));

    panel->addChild(new LabeledCheckbox(Vector2{ 20, Y + 1.5f * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
        "Show octree (may be slow)"));
    panel->addChild(new HR(Vector2{ 0, Y + 2.7f * spacing}, Vector2{ size.x, 0 }));

    Y = 110.0f;
    panel->addChild(new LabeledCheckbox(Vector2{ 20, Y }, Vector2{ size.x, styles::CHECKBOX_SIZE },
        "Enable blur effect (i.e. gases)"));
    panel->addChild(new LabeledCheckbox(Vector2{ 20, Y + 1 * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
         "Enable glow effect (i.e. fire)"));
    panel->addChild(new LabeledCheckbox(Vector2{ 20, Y + 2 * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
         "Enable shadows"));
    panel->addChild(new LabeledCheckbox(Vector2{ 20, Y + 3 * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
         "Enable ambient occlusion"));
    panel->addChild(new LabeledCheckbox(Vector2{ 20, Y + 4 * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
         "Enable transparency"));
    panel->addChild(new LabeledCheckbox(Vector2{ 20, Y + 5 * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
         "Enable reflections"));
    panel->addChild(new LabeledCheckbox(Vector2{ 20, Y + 6 * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
         "Enable refractions"));
    panel->addChild(new HR(Vector2{ 0, Y + 7.3f * spacing}, Vector2{ size.x, 0 }));

    Y = 310.0f;
    panel->addChild(new LabeledCheckbox(Vector2{ 20, Y }, Vector2{ size.x, styles::CHECKBOX_SIZE },
         "Fullscreen"));
    panel->addChild(new LabeledCheckbox(Vector2{ 20, Y + spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
         "Allow window resizing"));

}