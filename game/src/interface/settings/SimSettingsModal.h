#ifndef SIM_SETTING_MODAL_H
#define SIM_SETTING_MODAL_H

#include "../gui/components/Window.h"
#include "../gui/components/Checkbox.h"
#include "../gui/components/Dropdown.h"
#include "../gui/components/Slider.h"
#include "../gui/Style.h"

class Simulation;
class SimSettingsModal : public ui::Window {
public:
    SimSettingsModal(const Vector2 &pos, const Vector2 &size, Simulation * sim);
private:
    Simulation * sim = nullptr;

    ui::LabeledCheckbox
        * enableHeatCheckbox,
        * enableAirCheckbox;
    ui::Dropdown
        * gravityModeDropdown,
        * threadCountDropdown;
};

#endif