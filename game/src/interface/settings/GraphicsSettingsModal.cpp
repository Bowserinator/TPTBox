#include "GraphicsSettingsModal.h"
#include "../gui/components/TextButton.h"
#include "../gui/components/Checkbox.h"
#include "../gui/components/Dropdown.h"
#include "../gui/components/IconButton.h"
#include "../gui/components/Label.h"
#include "../gui/components/HR.h"
#include "../gui/components/Slider.h"
#include "../gui/styles.h"

#include "../../render/Renderer.h"
#include "data/SettingsData.h"

using namespace ui;

GraphicsSettingsModal::GraphicsSettingsModal(const Vector2 &pos, const Vector2 &size, Renderer * renderer):
    ui::Window(pos, size, ui::Window::Settings {
        .bottomPadding = styles::SETTINGS_BUTTON_HEIGHT,
        .interceptEvents = true,
        .title = "Graphics Settings"
    }, Style::getDefault()), renderer(renderer)
{
    auto settings = settings::data::ref()->graphics;
    addChild((new TextButton(
            Vector2{ 0, size.y - styles::SETTINGS_BUTTON_HEIGHT },
            Vector2{ size.x, styles::SETTINGS_BUTTON_HEIGHT },
            "OK"
        ))->setClickCallback([this]() {
            auto settings = settings::data::ref()->graphics;
            settings->renderMode = (Renderer::FragDebugMode)renderModeDropdown->selected();
            settings->showOctree = showOctreeCheckbox->checked();
            settings->enableTransparency = enableTransparencyCheckbox->checked();
            settings->enableReflection = enableReflectionsCheckbox->checked();
            settings->enableRefraction = enableRefractionsCheckbox->checked();
            settings->enableBlur = enableBlurCheckbox->checked();
            settings->enableGlow = enableGlowCheckbox->checked();
            settings->enableAO = enableAOCheckbox->checked();
            settings->enableShadows = enableShadowsCheckbox->checked();

            settings->aoStrength = aoStrengthSlider->getPercent();
            settings->shadowStrength = shadowStrengthSlider->getPercent();

            settings->fullScreen = fullscreenCheckbox->checked();
            this->renderer->update_settings(settings);
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

    constexpr float spacing = 26.0f;
    float Y = 40.0f;
    renderModeDropdown = (new Dropdown(
            Vector2{ size.x - styles::DROPDOWN_SIZE.x - 20.0f, Y },
            Vector2{ styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y })
        )
        ->addOption("Normal renderer", (int)Renderer::FragDebugMode::NODEBUG)
        ->addOption("Debug: step count", (int)Renderer::FragDebugMode::DEBUG_STEPS)
        ->addOption("Debug: normal vectors", (int)Renderer::FragDebugMode::DEBUG_NORMALS)
        ->addOption("Debug: ambient occlusion", (int)Renderer::FragDebugMode::DEBUG_AO)
        ->switchToOption(0);

    panel->addChild(renderModeDropdown);
    panel->addChild(new Label(
        Vector2{ 20.0f, Y },
        Vector2{ size.x - styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y },
        "Renderer mode"
    ));

    // Helpers
    auto addWarningLabel = [this, &size](const float Y, const Color &c, const char * s) {
        panel->addChild(new Label(Vector2{ 0, Y }, Vector2 {size.x - 20.0f, styles::CHECKBOX_SIZE},
            s, ui::Style{ .horizontalAlign = ui::Style::Align::Right, .textColor = c }));
    };
    auto createCheckboxAndAdd = [this, &size](const float Y, const char * s) {
        LabeledCheckbox * ptr = new LabeledCheckbox(Vector2{ 20, Y }, Vector2{ size.x, styles::CHECKBOX_SIZE }, s);
        panel->addChild(ptr);
        return ptr;
    };

    showOctreeCheckbox = createCheckboxAndAdd(Y + 1.5f * spacing, "Show octree (may be slow)");
    panel->addChild(new HR(Vector2{ 0, Y + 2.7f * spacing}, Vector2{ size.x, 0 }));

    Y = 120.0f;
    addWarningLabel(Y + 0 * spacing, YELLOW, "Medium Impact");
    addWarningLabel(Y + 1 * spacing, YELLOW, "Medium Impact");
    addWarningLabel(Y + 2 * spacing, GRAY, "Low Impact");
    addWarningLabel(Y + 3 * spacing, GRAY, "Low Impact");
    addWarningLabel(Y + 4 * spacing, RED, "High Impact");
    addWarningLabel(Y + 5 * spacing, RED, "High Impact");
    addWarningLabel(Y + 6 * spacing, RED, "High Impact");

    enableBlurCheckbox         = createCheckboxAndAdd(Y + 0 * spacing, "Enable blur effect (i.e. gases)");
    enableGlowCheckbox         = createCheckboxAndAdd(Y + 1 * spacing, "Enable glow effect (i.e. fire)");
    enableShadowsCheckbox      = createCheckboxAndAdd(Y + 2 * spacing, "Enable shadows");
    enableAOCheckbox           = createCheckboxAndAdd(Y + 3 * spacing, "Enable ambient occlusion (AO)");
    enableTransparencyCheckbox = createCheckboxAndAdd(Y + 4 * spacing, "Enable transparency");
    enableReflectionsCheckbox  = createCheckboxAndAdd(Y + 5 * spacing, "Enable reflections");
    enableRefractionsCheckbox  = createCheckboxAndAdd(Y + 6 * spacing, "Enable refractions");
    panel->addChild(new HR(Vector2{ 0, Y + 7.3f * spacing}, Vector2{ size.x, 0 }));

    Y = 320.0f;
    aoStrengthSlider     = new Slider(Vector2{ size.x / 2, Y + 0 * spacing }, Vector2{ size.x / 2 - 20.0f, 30 });
    shadowStrengthSlider = new Slider(Vector2{ size.x / 2, Y + 1 * spacing }, Vector2{ size.x / 2 - 20.0f, 30 });
    panel->addChild(new Label(Vector2{ 20.0f, Y + 0 * spacing }, Vector2{ size.x / 2 - 20.0f, 30 }, "AO Strength"));
    panel->addChild(new Label(Vector2{ 20.0f, Y + 1 * spacing }, Vector2{ size.x / 2 - 20.0f, 30 }, "Shadow Strength"));

    panel->addChild((new IconButton(Vector2{ size.x / 2 - 30.0f, Y + 0 * spacing }, Vector2{ 30.0f, 30 }, ICON_UNDO_FILL))
        ->setClickCallback([this]() { aoStrengthSlider->setPercent(settings::Graphics::defaultAOStrength); }));
    panel->addChild((new IconButton(Vector2{ size.x / 2 - 30.0f, Y + 1 * spacing }, Vector2{ 30.0f, 30 }, ICON_UNDO_FILL))
        ->setClickCallback([this]() { shadowStrengthSlider->setPercent(settings::Graphics::defaultShadowStrength); }));

    panel->addChild(aoStrengthSlider);
    panel->addChild(shadowStrengthSlider);
    panel->addChild(new HR(Vector2{ 0, Y + 2.5f * spacing}, Vector2{ size.x, 0 }));

    Y = 400.0f;
    fullscreenCheckbox = createCheckboxAndAdd(Y, "Fullscreen");

    // Update values from settings
    renderModeDropdown->switchToOption((int)settings->renderMode);
    showOctreeCheckbox->setChecked(settings->showOctree);
    enableTransparencyCheckbox->setChecked(settings->enableTransparency);
    enableReflectionsCheckbox->setChecked(settings->enableReflection);
    enableRefractionsCheckbox->setChecked(settings->enableRefraction);
    enableBlurCheckbox->setChecked(settings->enableBlur);
    enableGlowCheckbox->setChecked(settings->enableGlow);
    enableShadowsCheckbox->setChecked(settings->enableShadows);
    enableAOCheckbox->setChecked(settings->enableAO);
    aoStrengthSlider->setPercent(settings->aoStrength);
    shadowStrengthSlider->setPercent(settings->shadowStrength);

    fullscreenCheckbox->setChecked(settings->fullScreen);
}