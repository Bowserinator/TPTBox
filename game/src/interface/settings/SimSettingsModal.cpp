#include "SimSettingsModal.h"
#include "../gui/components/TextButton.h"
#include "../gui/components/Checkbox.h"
#include "../gui/components/Dropdown.h"
#include "../gui/components/IconButton.h"
#include "../gui/components/Label.h"
#include "../gui/components/HR.h"
#include "../gui/components/Slider.h"
#include "../gui/styles.h"

#include "../../simulation/Simulation.h"
#include "../../simulation/SimulationDef.h"
#include "data/SettingsData.h"

#include <omp.h>

using namespace ui;

SimSettingsModal::SimSettingsModal(const Vector2 &pos, const Vector2 &size, Simulation * sim):
    ui::Window(pos, size, ui::Window::Settings {
        .bottomPadding = styles::SETTINGS_BUTTON_HEIGHT,
        .interceptEvents = true,
        .title = "Simulation Settings"
    }, Style::getDefault()), sim(sim)
{
    auto settings = settings::data::ref()->sim;
    addChild((new TextButton(
            Vector2{ 0, size.y - styles::SETTINGS_BUTTON_HEIGHT },
            Vector2{ size.x, styles::SETTINGS_BUTTON_HEIGHT },
            "OK"
        ))->setClickCallback([this](unsigned int) {
            auto settings = settings::data::ref()->sim;
            settings->enableAir = enableAirCheckbox->checked();
            settings->enableHeat = enableHeatCheckbox->checked();
            settings->gravityMode = (GravityMode)gravityModeDropdown->selected();
            settings->threadCount = threadCountDropdown->selected();

            this->sim->update_settings(settings);
            settings::data::ref()->save_settings_to_file();
            tryClose(ui::Window::CloseReason::BUTTON);
        }));

    m_panel->addChild(new Label(
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
        m_panel->addChild(ptr);
        return ptr;
    };

    constexpr float spacing = 26.0f;
    float Y = 40.0f;
    enableHeatCheckbox = createCheckboxAndAdd(Y + 0.5f * spacing, "Enable heat simulation");
    enableAirCheckbox = createCheckboxAndAdd(Y + 1.5f * spacing, "Enable air simulation");

    float dropdownSizeScale = 0.75f;
    Y = 120.0f;
    gravityModeDropdown = (new Dropdown(
            Vector2{ size.x - styles::DROPDOWN_SIZE.x * dropdownSizeScale - 20.0f, Y },
            Vector2{ styles::DROPDOWN_SIZE.x * dropdownSizeScale, styles::DROPDOWN_SIZE.y })
        )
        ->addOption(sim->getGravityModeName(GravityMode::VERTICAL), (int)GravityMode::VERTICAL)
        ->addOption(sim->getGravityModeName(GravityMode::ZERO_G), (int)GravityMode::ZERO_G)
        ->addOption(sim->getGravityModeName(GravityMode::RADIAL), (int)GravityMode::RADIAL)
        ->switchToOption(0);

    m_panel->addChild(gravityModeDropdown);
    m_panel->addChild(new Label(
        Vector2{ 20.0f, Y },
        Vector2{ size.x - styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y },
        "Gravity Mode"
    ));

    Y += 1.35f * spacing;
    threadCountDropdown = (new Dropdown(
            Vector2{ size.x - styles::DROPDOWN_SIZE.x * dropdownSizeScale - 20.0f, Y },
            Vector2{ styles::DROPDOWN_SIZE.x * dropdownSizeScale, styles::DROPDOWN_SIZE.y })
        )
        ->addOption("Auto (1 per core)", -1)
        ->addOption("1", 1)
        ->addOption("2", 2)
        ->addOption("4", 4)
        ->addOption("8", 8)
        ->addOption(TextFormat("Max (%d)", MAX_SIM_THREADS), MAX_SIM_THREADS)
        ->switchToOption(-1);
    m_panel->addChild(threadCountDropdown);
    m_panel->addChild(new Label(
        Vector2{ 20.0f, Y },
        Vector2{ size.x - styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y },
        "Threads"
    ));
    m_panel->addChild(new Label(
        Vector2{ 20.0f, Y + 1.2f * spacing },
        Vector2{ size.x - styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y },
        TextFormat(
            "Max threads: Sim = %d  | hardware = %d",
            MAX_SIM_THREADS,
            omp_get_max_threads()
        ),
        Style { .horizontalAlign = Style::Align::Left, .textColor = BLUE }
    ));

    // Update values from settings
    enableHeatCheckbox->setChecked(settings->enableHeat);
    enableAirCheckbox->setChecked(settings->enableAir);
    gravityModeDropdown->switchToOption((int)settings->gravityMode);
    threadCountDropdown->switchToOption(settings->threadCount);
}
