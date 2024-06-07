#ifndef INTERFACE_SIM_MINI_PANELS_VIEWPANEL_H_
#define INTERFACE_SIM_MINI_PANELS_VIEWPANEL_H_

#include "../../../simulation/SimulationDef.h"
#include "../../gui/components/TextButton.h"
#include "../../gui/components/Panel.h"
#include "../../gui/components/Dropdown.h"
#include "../../gui/components/Label.h"
#include "../../gui/components/TextInput.h"

using namespace ui;

class Renderer;

class ViewPanel : public ui::Panel {
public:
    enum class ViewDropdownOptions { ALL = 0, SINGLE_LAYER, RANGE, ALL_BELOW, ALL_ABOVE };
    enum class AxisDropdownOptions { X = 0, Y, Z };

    ViewPanel(const Vector2 &pos, const Vector2 &size, Renderer * renderer);

private:
    unsigned int maxCurrentCoord() const;
    void clampCoordInput(ui::TextInput * input);
    void updateValues();

    void disableMin();
    void disableMax();
    void enableMin();
    void enableMax();

    Renderer * renderer;

    ui::TextInput
        * minCoordInput = nullptr,
        * maxCoordInput = nullptr;
    ui::Button
        * minButton = nullptr,
        * maxButton = nullptr;
    ui::Label
        * minLabel = nullptr,
        * maxLabel = nullptr;
    ui::Dropdown
        * axisDropdown = nullptr,
        * viewTypeDropdown = nullptr;
};

#endif // INTERFACE_SIM_MINI_PANELS_VIEWPANEL_H_
