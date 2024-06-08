#ifndef INTERFACE_SIM_MINI_PANELS_BRUSHPANEL_H_
#define INTERFACE_SIM_MINI_PANELS_BRUSHPANEL_H_

#include "../../../simulation/SimulationDef.h"
#include "../../gui/components/TextButton.h"
#include "../../gui/components/Panel.h"
#include "../../gui/components/Dropdown.h"
#include "../../gui/components/Label.h"
#include "../../gui/components/TextInput.h"

using namespace ui;

class BrushRenderer;

class BrushPanel : public ui::Panel {
public:
    BrushPanel(const Vector2 &pos, const Vector2 &size, BrushRenderer * brushRenderer);

private:
    void updateSizeValues();

    BrushRenderer * brushRenderer = nullptr;
    ui::Dropdown * brushTypeDropdown = nullptr;

    ui::TextInput * sizeXInput = nullptr;
    ui::TextInput * sizeYInput = nullptr;
    ui::TextInput * sizeZInput = nullptr;
};

#endif // INTERFACE_SIM_MINI_PANELS_BRUSHPANEL_H_
