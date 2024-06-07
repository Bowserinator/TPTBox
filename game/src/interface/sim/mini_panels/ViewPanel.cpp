#include "ViewPanel.h"
#include "../../../interface/settings/data/SettingsData.h"
#include "../../../render/camera/camera.h"
#include "../../../render/Renderer.h"
#include "../../../util/math.h"

using namespace ui;

ViewPanel::ViewPanel(const Vector2 &pos, const Vector2 &size, Renderer * renderer):
        ui::Panel(pos, size, Style::getDefault()), renderer(renderer) {
    constexpr float PAD = 5.0f;
    float Y = PAD * 2;

    auto updateInputOnDropdownChange = [this]() {
        ViewDropdownOptions viewOpt = (ViewDropdownOptions)viewTypeDropdown->selected();
        switch (viewOpt) {
            case ViewDropdownOptions::ALL:
                disableMin(); disableMax();
                // minCoordInput->setValue("0");
                maxCoordInput->setValue(std::to_string(maxCurrentCoord()));
                break;
            case ViewDropdownOptions::SINGLE_LAYER:
                enableMin(); disableMax();
                clampCoordInput(minCoordInput);
                break;
            case ViewDropdownOptions::RANGE:
                enableMin(); enableMax();
                clampCoordInput(minCoordInput);
                clampCoordInput(maxCoordInput);
                break;
            case ViewDropdownOptions::ALL_BELOW:
                disableMin(); enableMax();
                // minCoordInput->setValue("0");
                clampCoordInput(maxCoordInput);
                break;
            case ViewDropdownOptions::ALL_ABOVE:
                enableMin(); disableMax();
                clampCoordInput(minCoordInput);
                // maxCoordInput->setValue(std::to_string(maxCurrentCoord()));
                break;
        }
        updateValues();
    };

    viewTypeDropdown = (new Dropdown(
            Vector2{ PAD, Y },
            Vector2{ size.x * 0.7f - 2 * PAD, styles::DROPDOWN_SIZE.y })
        )
        ->addOption("All", (int)ViewDropdownOptions::ALL)
        ->addOption("Single Layer", (int)ViewDropdownOptions::SINGLE_LAYER)
        ->addOption("Range", (int)ViewDropdownOptions::RANGE)
        ->addOption("All Below", (int)ViewDropdownOptions::ALL_BELOW)
        ->addOption("All Above", (int)ViewDropdownOptions::ALL_ABOVE)
        ->setUpdateCallback([this, updateInputOnDropdownChange](int _choice) { updateInputOnDropdownChange(); })
        ->switchToOption((int)ViewDropdownOptions::ALL);
    addChild(viewTypeDropdown);

    axisDropdown = (new Dropdown(
            Vector2{ PAD + viewTypeDropdown->size.x + PAD, Y },
            Vector2{ size.x * 0.3f - PAD, styles::DROPDOWN_SIZE.y })
        )
        ->addOption("X", (int)AxisDropdownOptions::X)
        ->addOption("Y", (int)AxisDropdownOptions::Y)
        ->addOption("Z", (int)AxisDropdownOptions::Z)
        ->switchToOption((int)AxisDropdownOptions::X)
        ->setUpdateCallback([this, updateInputOnDropdownChange](int _choice) { updateInputOnDropdownChange(); });
    addChild(axisDropdown);

    Y += styles::DROPDOWN_SIZE.y + 2 * PAD;
    auto validateCoord = [this](const std::string &s) -> bool {
        if (!s.length()) return false;
        auto val = std::stoi(s);
        return 0 <= val && maxCurrentCoord() >= val;
    };
    auto createInputGroup = [this, &validateCoord, size](const std::string &label, float Y) {
        auto labelObj = new Label(
            Vector2{ PAD, Y },
            Vector2{ 40.0f, styles::DROPDOWN_SIZE.y },
            label
        );
        addChild(labelObj);

        auto textInput = (new TextInput(
                Vector2{ PAD + 40.0f, Y },
                Vector2{ size.x * 0.6f, styles::DROPDOWN_SIZE.y })
            )
                ->setMaxLength(3)->setPlaceholder("0")
                ->setInputAllowed([](const std::string &s) { return s.length() == 1 && isdigit(s[0]); })
                ->setInputValidation(validateCoord)
                ->setOnValueChange([this](const std::string &val) {
                    updateValues();
                });
        addChild(textInput);

        auto btn = (new TextButton(
            Vector2{ 2.0f * PAD + 40.0f + size.x * 0.6f, Y },
            Vector2{ size.x * 0.4f - 3 * PAD - 40.0f, styles::DROPDOWN_SIZE.y },
            "+-"
        ))->setClickCallback([this, textInput](unsigned int button) {
            if (textInput->isInputValid()) {
                auto val = std::stoi(textInput->getValue());
                if (button == MOUSE_BUTTON_LEFT) val++;
                else if (button == MOUSE_BUTTON_RIGHT) val--;

                val = util::clamp(val, 0, maxCurrentCoord());
                textInput->setValue(std::to_string(val));
                updateValues();
            }
        });
        addChild(btn);

        return std::make_tuple(labelObj, textInput, btn);
    };

    auto g1 = createInputGroup("Min", Y);
    minLabel = std::get<0>(g1);
    minCoordInput = std::get<1>(g1);
    minButton = std::get<2>(g1);
    Y += styles::DROPDOWN_SIZE.y + PAD;

    auto g2 = createInputGroup("Max", Y);
    maxLabel = std::get<0>(g2);
    maxCoordInput = std::get<1>(g2);
    maxButton = std::get<2>(g2);
    Y += styles::DROPDOWN_SIZE.y + PAD;

    minCoordInput->nextTabInput = maxCoordInput;
    maxCoordInput->prevTabInput = minCoordInput;

    // Default values
    minCoordInput->setValue("0");
    maxCoordInput->setValue(std::to_string(XRES));
    disableMin();
    disableMax();

    addChild((new TextButton(
        Vector2{ PAD, size.y - styles::SETTINGS_BUTTON_HEIGHT - PAD },
        Vector2{ size.x - 2 * PAD, styles::SETTINGS_BUTTON_HEIGHT },
        "Set here"
    ))->setClickCallback([this, renderer](unsigned int) {
        float valf = 0.0f;
        switch ((AxisDropdownOptions)axisDropdown->selected()) {
            case AxisDropdownOptions::X: valf = renderer->get_cam()->camera.position.x; break;
            case AxisDropdownOptions::Y: valf = renderer->get_cam()->camera.position.y; break;
            case AxisDropdownOptions::Z: valf = renderer->get_cam()->camera.position.z; break;
        }
        unsigned int val = (unsigned int)util::clamp((int)valf, 0, (int)maxCurrentCoord());

        ViewDropdownOptions viewOpt = (ViewDropdownOptions)viewTypeDropdown->selected();
        switch (viewOpt) {
            case ViewDropdownOptions::ALL:
                break;
            case ViewDropdownOptions::SINGLE_LAYER:
                minCoordInput->setValue(std::to_string(val));
                break;
            case ViewDropdownOptions::RANGE: {
                float minVal = !minCoordInput->isInputValid() ? 0.0f : std::stoi(minCoordInput->getValue());
                float maxVal = !maxCoordInput->isInputValid() ? maxCurrentCoord() : std::stoi(maxCoordInput->getValue());
                float midVal = (minVal + maxVal) / 2;
                
                if (val < midVal)
                    minCoordInput->setValue(std::to_string(val));
                else
                    maxCoordInput->setValue(std::to_string(val));
                break;
            }
            case ViewDropdownOptions::ALL_BELOW:
                maxCoordInput->setValue(std::to_string(val));
                break;
            case ViewDropdownOptions::ALL_ABOVE:
                minCoordInput->setValue(std::to_string(val));
                break;
        }

        std::cout << renderer->get_cam()->camera.position.y << '\n';
    }));
}

unsigned int ViewPanel::maxCurrentCoord() const {
    switch ((AxisDropdownOptions)axisDropdown->selected()) {
        case AxisDropdownOptions::X: return XRES;
        case AxisDropdownOptions::Y: return YRES;
        case AxisDropdownOptions::Z: return ZRES;
    }
    return 0;
}

void ViewPanel::clampCoordInput(ui::TextInput * input) {
    if (input->isInputValid()) {
        auto val = std::stoi(input->getValue());
        if (val > maxCurrentCoord()) {
            val = maxCurrentCoord();
            input->setValue(std::to_string(val));
        }
    }
}

void ViewPanel::updateValues() {
    auto settings = settings::data::ref()->graphics;
    float viewSliceBegin[] = { 0.0f, 0.0f, 0.0f };
    float viewSliceEnd[] = { (float)XRES, (float)YRES, (float)ZRES };

    ViewDropdownOptions viewOpt = (ViewDropdownOptions)viewTypeDropdown->selected();
    int axis = axisDropdown->selected();

    switch (viewOpt) {
        case ViewDropdownOptions::ALL:
            break;
        case ViewDropdownOptions::SINGLE_LAYER:
            if (minCoordInput->isInputValid()) {
                int val = std::stoi(minCoordInput->getValue());
                viewSliceBegin[axis] = val;
                viewSliceEnd[axis] = val;
            }
            break;
        case ViewDropdownOptions::RANGE: {
            float minVal = !minCoordInput->isInputValid() ? viewSliceBegin[axis] : std::stoi(minCoordInput->getValue());
            float maxVal = !maxCoordInput->isInputValid() ? viewSliceEnd[axis] : std::stoi(maxCoordInput->getValue());
            if (minVal > maxVal) std::swap(minVal, maxVal);

            viewSliceBegin[axis] = minVal;
            viewSliceEnd[axis] = maxVal;
            break;
        }
        case ViewDropdownOptions::ALL_BELOW:
            if (maxCoordInput->isInputValid())
                viewSliceEnd[axis] = std::stoi(maxCoordInput->getValue());
            break;
        case ViewDropdownOptions::ALL_ABOVE:
            if (minCoordInput->isInputValid())
                viewSliceBegin[axis] = std::stoi(minCoordInput->getValue());
            break;
    }

    settings->viewSliceBegin = Vector3{ viewSliceBegin[0], viewSliceBegin[1], viewSliceBegin[2] };
    settings->viewSliceEnd = Vector3{ viewSliceEnd[0],viewSliceEnd[1], viewSliceEnd[2] };
    renderer->update_settings(settings);
}

void ViewPanel::disableMin() {
    minButton->disable();
    minCoordInput->disable();
    minLabel->disable();
}

void ViewPanel::disableMax() {
    maxButton->disable();
    maxCoordInput->disable();
    maxLabel->disable();
}

void ViewPanel::enableMin() {
    minButton->enable();
    minCoordInput->enable(); 
    minLabel->enable();
}

void ViewPanel::enableMax() {
    maxButton->enable();
    maxCoordInput->enable(); 
    maxLabel->enable();
}
