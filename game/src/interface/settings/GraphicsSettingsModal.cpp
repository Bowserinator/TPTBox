#include "GraphicsSettingsModal.h"
#include "../gui/components/TextButton.h"
#include "../gui/components/Checkbox.h"
#include "../gui/components/Dropdown.h"
#include "../gui/components/Label.h"
#include "../gui/components/HR.h"
#include "../gui/styles.h"

#include "../../render/Renderer.h"
#include "data/SettingsData.h"

using namespace ui;

GraphicsSettingsModal::GraphicsSettingsModal(const Vector2 &pos, const Vector2 &size, Renderer * renderer):
    ui::Window(pos, size, "Graphics Settings", Style::getDefault()), renderer(renderer)
{
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

            settings->fullScreen = fullscreenCheckbox->checked();
            settings->allowResizing = resizableCheckbox->checked();
            this->renderer->update_settings(settings);
        }));

    panel->addChild(new Label(
        Vector2{ 20.0f, 0.0f },
        Vector2{ size.x, 20.0f },
        "Settings will apply once you hit \"OK\"",
        Style {
            .horizontalAlign = Style::Align::Left,
            .textColor = YELLOW
        }
    ));

    constexpr float spacing = 26.0f;
    float Y = 30.0f;
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

    showOctreeCheckbox = new LabeledCheckbox(Vector2{ 20, Y + 1.5f * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
        "Show octree (may be slow)");
    panel->addChild(showOctreeCheckbox);
    panel->addChild(new HR(Vector2{ 0, Y + 2.7f * spacing}, Vector2{ size.x, 0 }));

    Y = 110.0f;
    enableBlurCheckbox = new LabeledCheckbox(Vector2{ 20, Y }, Vector2{ size.x, styles::CHECKBOX_SIZE },
        "Enable blur effect (i.e. gases)");
    enableGlowCheckbox = new LabeledCheckbox(Vector2{ 20, Y + 1 * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
        "Enable glow effect (i.e. fire)");
    enableShadowsCheckbox = new LabeledCheckbox(Vector2{ 20, Y + 2 * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
        "Enable shadows");
    enableAOCheckbox = new LabeledCheckbox(Vector2{ 20, Y + 3 * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
        "Enable ambient occlusion");
    enableTransparencyCheckbox = new LabeledCheckbox(Vector2{ 20, Y + 4 * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
        "Enable transparency");
    enableReflectionsCheckbox = new LabeledCheckbox(Vector2{ 20, Y + 5 * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
        "Enable reflections");
    enableRefractionsCheckbox = new LabeledCheckbox(Vector2{ 20, Y + 6 * spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
        "Enable refractions");

    panel->addChild(enableBlurCheckbox);
    panel->addChild(enableGlowCheckbox);
    panel->addChild(enableShadowsCheckbox);
    panel->addChild(enableAOCheckbox);
    panel->addChild(enableTransparencyCheckbox);
    panel->addChild(enableReflectionsCheckbox);
    panel->addChild(enableRefractionsCheckbox);
    panel->addChild(new HR(Vector2{ 0, Y + 7.3f * spacing}, Vector2{ size.x, 0 }));

    Y = 310.0f;
    fullscreenCheckbox = new LabeledCheckbox(Vector2{ 20, Y }, Vector2{ size.x, styles::CHECKBOX_SIZE }, "Fullscreen");
    resizableCheckbox = new LabeledCheckbox(Vector2{ 20, Y + spacing }, Vector2{ size.x, styles::CHECKBOX_SIZE },
        "Allow window resizing");

    panel->addChild(fullscreenCheckbox);
    panel->addChild(resizableCheckbox);


    // Update values from settings
    auto settings = settings::data::ref()->graphics;
    renderModeDropdown->switchToOption((int)settings->renderMode);
    showOctreeCheckbox->setChecked(settings->showOctree);
    enableTransparencyCheckbox->setChecked(settings->enableTransparency);
    enableReflectionsCheckbox->setChecked(settings->enableReflection);
    enableRefractionsCheckbox->setChecked(settings->enableRefraction);
    enableBlurCheckbox->setChecked(settings->enableBlur);
    enableGlowCheckbox->setChecked(settings->enableGlow);
    enableShadowsCheckbox->setChecked(settings->enableShadows);
    enableAOCheckbox->setChecked(settings->enableAO);

    fullscreenCheckbox->setChecked(settings->fullScreen);
    resizableCheckbox->setChecked(settings->allowResizing);

}