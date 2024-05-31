#ifndef GRAPHICS_SETTING_MODAL_H
#define GRAPHICS_SETTING_MODAL_H

#include "../gui/components/Window.h"
#include "../gui/components/Checkbox.h"
#include "../gui/components/Dropdown.h"
#include "../gui/components/Slider.h"
#include "../gui/components/TextInput.h"
#include "../gui/components/ColorPicker.h"
#include "../gui/Style.h"

class Renderer;
class GraphicsSettingsModal : public ui::Window {
public:
    GraphicsSettingsModal(const Vector2 &pos, const Vector2 &size, Renderer * renderer);
private:
    Renderer * renderer = nullptr;

    ui::LabeledCheckbox 
        * showOctreeCheckbox,
        * enableOutlineCheckbox,
        * enableBlurCheckbox,
        * enableGlowCheckbox,
        * enableShadowsCheckbox,
        * enableAOCheckbox,
        * enableTransparencyCheckbox,
        * enableReflectionsCheckbox,
        * enableRefractionsCheckbox,
        * fullscreenCheckbox;
    ui::Dropdown * renderModeDropdown;
    ui::Slider * aoStrengthSlider,
        * shadowStrengthSlider;
    ui::TextInput
        * heatMinTextInput,
        * heatMaxTextInput,
        * renderDownscaleTextInput;
    ui::ColorPicker
        * bgColorPicker,
        * shadowColorPicker;
};

#endif