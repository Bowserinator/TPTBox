#include "UISettingsModal.h"
#include "../gui/components/TextButton.h"
#include "../gui/components/Checkbox.h"
#include "../gui/components/Dropdown.h"
#include "../gui/components/IconButton.h"
#include "../gui/components/Label.h"
#include "../gui/components/HR.h"
#include "../gui/components/Slider.h"
#include "../gui/styles.h"

#include "data/SettingsData.h"

using namespace ui;

UISettingsModal::UISettingsModal(const Vector2 &pos, const Vector2 &size):
    ui::Window(pos, size, ui::Window::Settings {
        .bottomPadding = styles::SETTINGS_BUTTON_HEIGHT,
        .interceptEvents = true,
        .title = "UI Settings"
    }, Style::getDefault())
{
    auto settings = settings::data::ref()->ui;
    addChild((new TextButton(
            Vector2{ 0, size.y - styles::SETTINGS_BUTTON_HEIGHT },
            Vector2{ size.x, styles::SETTINGS_BUTTON_HEIGHT },
            "OK"
        ))->setClickCallback([this]() {
            auto settings = settings::data::ref()->ui;
            settings->hideHud = hideHUDCheckbox->checked();
            settings->oppositeTool = oppositeToolCheckbox->checked();
            settings->frameIndependentCam = frameIndependentCamCheckbox->checked(),
            settings->fastQuit = fastQuitCheckbox->checked();

            settings->movementMode = (settings::UI::MovementMode)movementModeDropdown->selected();
            settings->temperatureUnit = (settings::UI::TemperatureUnit)temperatureDropdown->selected();

            tryClose(ui::Window::CloseReason::BUTTON);
        }));

    panel->addChild(new Label(
        Vector2{ 20.0f, 10.0f },
        Vector2{ size.x, 20.0f },
        "Settings will apply once you hit \"OK\"",
        Style {
            .horizontalAlign = Style::Align::Left,
            .textColor = YELLOW
        }
    ));

    auto createCheckboxAndAdd = [this, &size](const float Y, const char * s) {
        LabeledCheckbox * ptr = new LabeledCheckbox(Vector2{ 20, Y }, Vector2{ size.x, styles::CHECKBOX_SIZE }, s);
        panel->addChild(ptr);
        return ptr;
    };

    constexpr float spacing = 26.0f;
    float Y = 40.0f;
    hideHUDCheckbox = createCheckboxAndAdd(Y + 0.0f * spacing, "Hide HUD when mouse is near top");
    oppositeToolCheckbox = createCheckboxAndAdd(Y + 1.0f * spacing, "Auto-set opposite tool");
    frameIndependentCamCheckbox = createCheckboxAndAdd(Y + 2.0f * spacing, "Frame-independent camera");
    fastQuitCheckbox = createCheckboxAndAdd(Y + 3.0f * spacing, "Fast quit (no confirmation)");

    Y = 40.0f + 4.5 * spacing;
    panel->addChild(new Label(
        Vector2{ 20.0f, Y },
        Vector2{ size.x - styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y },
        "Temperature Scale"
    ));
    temperatureDropdown = (new Dropdown(
            Vector2{ size.x - styles::DROPDOWN_SIZE.x * 0.75f - 20.0f, Y },
            Vector2{ styles::DROPDOWN_SIZE.x * 0.75f, styles::DROPDOWN_SIZE.y })
        )
        ->addOption("Celsius (C)", (int)settings::UI::TemperatureUnit::C)
        ->addOption("Kelvin (K)", (int)settings::UI::TemperatureUnit::K)
        ->addOption("Fahrenheit (F)", (int)settings::UI::TemperatureUnit::F)
        ->switchToOption(0);
    panel->addChild(temperatureDropdown);

    panel->addChild(new Label(
        Vector2{ 20.0f, Y + 1.25f * spacing },
        Vector2{ size.x - styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y },
        "Camera Controls"
    ));
    movementModeDropdown = (new Dropdown(
            Vector2{ size.x - styles::DROPDOWN_SIZE.x * 0.75f - 20.0f, Y + 1.25f * spacing },
            Vector2{ styles::DROPDOWN_SIZE.x * 0.75f, styles::DROPDOWN_SIZE.y })
        )
        ->addOption("3D Editor", (int)settings::UI::MovementMode::THREED)
        ->addOption("First-person", (int)settings::UI::MovementMode::FIRST_PERSON)
        ->switchToOption(0);
    panel->addChild(movementModeDropdown);

    // Update values from settings
    hideHUDCheckbox->setChecked(settings->hideHud);
    oppositeToolCheckbox->setChecked(settings->oppositeTool);
    frameIndependentCamCheckbox->setChecked(settings->frameIndependentCam),
    fastQuitCheckbox->setChecked(settings->fastQuit);
    movementModeDropdown->switchToOption((int)settings->movementMode);
    temperatureDropdown->switchToOption((int)settings->temperatureUnit);
}
