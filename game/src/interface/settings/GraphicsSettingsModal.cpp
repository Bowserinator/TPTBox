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
#include "../../util/str_format.h"
#include "data/SettingsData.h"

#include <string>

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
        ))->setClickCallback([this](unsigned int _button) {
            auto settings = settings::data::ref()->graphics;
            settings->renderMode = (Renderer::FragDebugMode)renderModeDropdown->selected();
            settings->showOctree = showOctreeCheckbox->checked();
            settings->showOutline = enableOutlineCheckbox->checked();
            settings->enableTransparency = enableTransparencyCheckbox->checked();
            settings->enableReflection = enableReflectionsCheckbox->checked();
            settings->enableRefraction = enableRefractionsCheckbox->checked();
            settings->enableBlur = enableBlurCheckbox->checked();
            settings->enableGlow = enableGlowCheckbox->checked();
            settings->enableAO = enableAOCheckbox->checked();
            settings->enableShadows = enableShadowsCheckbox->checked();

            settings->aoStrength = aoStrengthSlider->percent();
            settings->shadowStrength = shadowStrengthSlider->percent();

            settings->fullScreen = fullscreenCheckbox->checked();
            if (heatMinTextInput->isInputValid())
                settings->heatViewMin = util::temp_string_to_kelvin(heatMinTextInput->value()).value();
            if (heatMaxTextInput->isInputValid())
                settings->heatViewMax = util::temp_string_to_kelvin(heatMaxTextInput->value()).value();
            if (renderDownscaleTextInput->isInputValid())
                settings->renderDownscale = std::stof(renderDownscaleTextInput->value());

            settings->backgroundColor = bgColorPicker->value();
            settings->shadowColor = shadowColorPicker->value();

            this->renderer->update_settings(settings);
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

    m_panel->addChild(renderModeDropdown);
    m_panel->addChild(new Label(
        Vector2{ 20.0f, Y },
        Vector2{ size.x - styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y },
        "Renderer mode"
    ));

    // Helpers
    auto addWarningLabel = [this, &size](const float Y, const Color &c, const char * s) {
        m_panel->addChild(new Label(Vector2{ 0, Y }, Vector2 {size.x - 20.0f, styles::CHECKBOX_SIZE},
            s, ui::Style{ .horizontalAlign = ui::Style::Align::Right, .textColor = c }));
    };
    auto createCheckboxAndAdd = [this, &size](const float Y, const char * s) {
        LabeledCheckbox * ptr = new LabeledCheckbox(Vector2{ 20, Y }, Vector2{ size.x, styles::CHECKBOX_SIZE }, s);
        m_panel->addChild(ptr);
        return ptr;
    };

    showOctreeCheckbox = createCheckboxAndAdd(Y + 1.5f * spacing, "Show octree (may be slow)");
    enableOutlineCheckbox = createCheckboxAndAdd(Y + 2.5f * spacing, "Show voxel outlines");
    m_panel->addChild(new HR(Vector2{ 0, Y + 3.7f * spacing}, Vector2{ size.x, 0 }));

    Y = 120.0f + spacing;
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
    m_panel->addChild(new HR(Vector2{ 0, Y + 7.3f * spacing}, Vector2{ size.x, 0 }));

    Y = 320.0f + spacing;
    aoStrengthSlider     = new Slider(Vector2{ size.x / 2, Y + 0 * spacing }, Vector2{ size.x / 2 - 20.0f, 30 });
    shadowStrengthSlider = new Slider(Vector2{ size.x / 2, Y + 1 * spacing }, Vector2{ size.x / 2 - 20.0f, 30 });
    m_panel->addChild(new Label(Vector2{ 20.0f, Y + 0 * spacing }, Vector2{ size.x / 2 - 20.0f, 30 }, "AO Strength"));
    m_panel->addChild(new Label(Vector2{ 20.0f, Y + 1 * spacing }, Vector2{ size.x / 2 - 20.0f, 30 }, "Shadow Strength"));

    m_panel->addChild((new IconButton(Vector2{ size.x / 2 - 30.0f, Y + 0 * spacing },
        Vector2{ 30.0f, 30 }, ICON_UNDO_FILL))
        ->setClickCallback([this](unsigned int) {
            aoStrengthSlider->setPercent(settings::Graphics::defaultAOStrength); }));
    m_panel->addChild((new IconButton(Vector2{ size.x / 2 - 30.0f, Y + 1 * spacing },
        Vector2{ 30.0f, 30 }, ICON_UNDO_FILL))
        ->setClickCallback([this](unsigned int) {
            shadowStrengthSlider->setPercent(settings::Graphics::defaultShadowStrength); }));

    m_panel->addChild(aoStrengthSlider);
    m_panel->addChild(shadowStrengthSlider);
    m_panel->addChild(new HR(Vector2{ 0, Y + 2.5f * spacing}, Vector2{ size.x, 0 }));

    Y = 400.0f + spacing;
    fullscreenCheckbox = createCheckboxAndAdd(Y, "Fullscreen");

    // Temperature
    Y = 450.0f + spacing;
    m_panel->addChild(new HR(Vector2{ 0, Y - 0.5f * spacing }, Vector2{ size.x, 0 }));

    auto validate_temp = [](const std::string &s) -> bool {
        auto val = util::temp_string_to_kelvin(s);
        if (!val.has_value()) return false;
        return MIN_TEMP <= val && MAX_TEMP >= val;
    };
    auto float_input_allowed = [](const std::string &s) -> bool {
        return s.length() == 1 && (isdigit(s[0]) || s[0] == '.');
    };
    auto temp_input_allowed = [float_input_allowed](const std::string &s) -> bool {
        return float_input_allowed(s) ||
            s == "K" || s == "k" ||
            s == "C" || s == "c" ||
            s == "F" || s == "f";
    };

    m_panel->addChild(new Label(
        Vector2{ 20.0f, Y + 0 * 1.25f * spacing },
        Vector2{ size.x - styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y },
        "Heat view min (K)"
    ));
    heatMinTextInput = (new TextInput(
            Vector2{ size.x - styles::DROPDOWN_SIZE.x * 0.75f - 20.0f, Y + 0 * 1.25f * spacing },
            Vector2{ styles::DROPDOWN_SIZE.x * 0.75f, styles::DROPDOWN_SIZE.y })
        )
            ->setMaxLength(16)->setPlaceholder("0.0")
            ->setInputAllowed(temp_input_allowed)
            ->setInputValidation(validate_temp);
    m_panel->addChild(heatMinTextInput);

    m_panel->addChild(new Label(
        Vector2{ 20.0f, Y + 1 * 1.25f * spacing },
        Vector2{ size.x - styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y },
        "Heat view max (K)"
    ));
    heatMaxTextInput = (new TextInput(
            Vector2{ size.x - styles::DROPDOWN_SIZE.x * 0.75f - 20.0f, Y + 1 * 1.25f * spacing },
            Vector2{ styles::DROPDOWN_SIZE.x * 0.75f, styles::DROPDOWN_SIZE.y })
        )
            ->setMaxLength(16)->setPlaceholder(std::format("{:.2f}K", settings::Graphics::defaultHeatViewMax))
            ->setInputAllowed(temp_input_allowed)
            ->setInputValidation(validate_temp);
    m_panel->addChild(heatMaxTextInput);

    m_panel->addChild((new IconButton(Vector2{ size.x / 2 - 30.0f, Y + 0 * 1.25f * spacing },
        Vector2{ 30.0f, 30 }, ICON_UNDO_FILL))
        ->setClickCallback([this](unsigned int) {
            heatMinTextInput->setValue(std::format("{:.2f}K", MIN_TEMP)); }));
    m_panel->addChild((new IconButton(Vector2{ size.x / 2 - 30.0f, Y + 1 * 1.25f * spacing },
        Vector2{ 30.0f, 30 }, ICON_UNDO_FILL))
        ->setClickCallback([this](unsigned int) {
            heatMaxTextInput->setValue(std::format("{:.2f}K", settings::Graphics::defaultHeatViewMax)); }));

    // Render downscale
    auto validate_downscale = [](const std::string &s) -> bool {
        auto val = util::parse_string_float(s);
        if (!val.has_value()) return false;
        return 1.0f <= val && val <= 5.0f;
    };

    m_panel->addChild(new Label(
        Vector2{ 20.0f, Y + 2 * 1.25f * spacing },
        Vector2{ size.x - styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y },
        "Render downscale"
    ));
    m_panel->addChild(new Label(
        Vector2{ 20.0f, Y + spacing + 2.15f * 1.25f * spacing },
        Vector2{ size.x, styles::DROPDOWN_SIZE.y },
        "Higher number = lower resolution (1-5x)",
        Style{ .horizontalAlign = Style::Align::Left }.setAllTextColors(GRAY)
    ));
    renderDownscaleTextInput = (new TextInput(
            Vector2{ size.x - styles::DROPDOWN_SIZE.x * 0.75f - 20.0f, Y + 2 * 1.25f * spacing },
            Vector2{ styles::DROPDOWN_SIZE.x * 0.75f, styles::DROPDOWN_SIZE.y })
        )
            ->setMaxLength(4)->setPlaceholder("1.5")
            ->setInputAllowed(float_input_allowed)
            ->setInputValidation(validate_downscale);
    m_panel->addChild(renderDownscaleTextInput);
    m_panel->addChild((new IconButton(Vector2{ size.x / 2 - 30.0f, Y + 2 * 1.25f * spacing },
    Vector2{ 30.0f, 30 }, ICON_UNDO_FILL))
        ->setClickCallback([this](unsigned int) {
            renderDownscaleTextInput->setValue(std::format("{:.1f}", settings::Graphics::defaultRenderDownscale)); }));

    heatMinTextInput->nextTabInput = heatMaxTextInput;
    heatMaxTextInput->nextTabInput = renderDownscaleTextInput;
    renderDownscaleTextInput->prevTabInput = heatMaxTextInput;
    heatMaxTextInput->prevTabInput = heatMinTextInput;

    // Color pickers
    m_panel->addChild(new Label(
        Vector2{ 20.0f, Y + 4 * 1.25f * spacing },
        Vector2{ size.x - styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y },
        "Background Color"
    ));
    bgColorPicker = new ColorPicker(
            Vector2{ size.x - styles::DROPDOWN_SIZE.x * 0.75f - 20.0f, Y + 4 * 1.25f * spacing },
            Vector2{ styles::DROPDOWN_SIZE.x * 0.75f, styles::DROPDOWN_SIZE.y });
    m_panel->addChild(bgColorPicker->noAlpha());
    m_panel->addChild((new IconButton(Vector2{ size.x / 2 - 30.0f, Y + 4 * 1.25f * spacing },
        Vector2{ 30.0f, 30 }, ICON_UNDO_FILL))
        ->setClickCallback([this](unsigned int) {
            bgColorPicker->setValue(settings::Graphics::defaultBackgroundColor); }));

    m_panel->addChild(new Label(
        Vector2{ 20.0f, Y + 5 * 1.25f * spacing },
        Vector2{ size.x - styles::DROPDOWN_SIZE.x, styles::DROPDOWN_SIZE.y },
        "Shadow Color"
    ));
    shadowColorPicker = new ColorPicker(
            Vector2{ size.x - styles::DROPDOWN_SIZE.x * 0.75f - 20.0f, Y + 5 * 1.25f * spacing },
            Vector2{ styles::DROPDOWN_SIZE.x * 0.75f, styles::DROPDOWN_SIZE.y });
    m_panel->addChild(shadowColorPicker->noAlpha());
    m_panel->addChild((new IconButton(Vector2{ size.x / 2 - 30.0f, Y + 5 * 1.25f * spacing },
        Vector2{ 30.0f, 30 }, ICON_UNDO_FILL))
        ->setClickCallback([this](unsigned int) {
            shadowColorPicker->setValue(settings::Graphics::defaultShadowColor); }));

    // Update values from settings
    renderModeDropdown->switchToOption((int)settings->renderMode);
    showOctreeCheckbox->setChecked(settings->showOctree);
    enableOutlineCheckbox->setChecked(settings->showOutline);
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
    heatMinTextInput->setValue(std::format("{:.2f}K", settings->heatViewMin));
    heatMaxTextInput->setValue(std::format("{:.2f}K", settings->heatViewMax));
    renderDownscaleTextInput->setValue(std::format("{:.1f}", settings->renderDownscale));
    bgColorPicker->setValue(settings->backgroundColor);
    shadowColorPicker->setValue(settings->shadowColor);
}
