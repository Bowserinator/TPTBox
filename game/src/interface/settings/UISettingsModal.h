#ifndef INTERFACE_SETTINGS_UISETTINGSMODAL_H_
#define INTERFACE_SETTINGS_UISETTINGSMODAL_H_

#include "../gui/components/Window.h"
#include "../gui/components/Checkbox.h"
#include "../gui/components/Dropdown.h"
#include "../gui/components/Slider.h"
#include "../gui/Style.h"

class Simulation;
class UISettingsModal : public ui::Window {
public:
    UISettingsModal(const Vector2 &pos, const Vector2 &size);
private:
    ui::LabeledCheckbox
        * hideHUDCheckbox,
        * oppositeToolCheckbox,
        * frameIndependentCamCheckbox,
        * fastQuitCheckbox;
    ui::Dropdown
        * movementModeDropdown,
        * temperatureDropdown;
    ui::Slider * mouseSensitivitySlider;
};

#endif // INTERFACE_SETTINGS_UISETTINGSMODAL_H_
