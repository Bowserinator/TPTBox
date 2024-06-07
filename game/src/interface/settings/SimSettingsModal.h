#ifndef INTERFACE_SETTINGS_SIMSETTINGSMODAL_H_
#define INTERFACE_SETTINGS_SIMSETTINGSMODAL_H_

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

#endif // INTERFACE_SETTINGS_SIMSETTINGSMODAL_H_
