#ifndef GRAPHICS_SETTING_MODAL_H
#define GRAPHICS_SETTING_MODAL_H

#include "../gui/components/Window.h"
#include "../gui/components/Checkbox.h"
#include "../gui/components/Dropdown.h"
#include "../gui/components/Slider.h"
#include "../gui/Style.h"

class Renderer;
class GraphicsSettingsModal : public ui::Window {
public:
    GraphicsSettingsModal(const Vector2 &pos, const Vector2 &size, Renderer * renderer);

    virtual void tryClose(CloseReason reason) override {
        if (reason == ui::Window::CloseReason::BUTTON)
            close();
    }
private:
    Renderer * renderer = nullptr;

    ui::LabeledCheckbox * showOctreeCheckbox,
        * enableBlurCheckbox,
        * enableGlowCheckbox,
        * enableShadowsCheckbox,
        * enableAOCheckbox,
        * enableTransparencyCheckbox,
        * enableReflectionsCheckbox,
        * enableRefractionsCheckbox,
        * fullscreenCheckbox,
        * resizableCheckbox;
    ui::Dropdown * renderModeDropdown;
    ui::Slider * aoStrengthSlider,
        * shadowStrengthSlider;
};

#endif