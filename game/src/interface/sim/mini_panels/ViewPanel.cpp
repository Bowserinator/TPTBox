#include "ViewPanel.h"
#include "../../../util/math.h"

using namespace ui;

ViewPanel::ViewPanel(const Vector2 &pos, const Vector2 &size): ui::Panel(pos, size, Style::getDefault()) {
    constexpr float PAD = 5.0f;
    float Y = PAD * 2;

    auto updateInputOnDropdownChange = [this]() {
        ViewDropdownOptions viewOpt = (ViewDropdownOptions)viewTypeDropdown->selected();
        switch (viewOpt) {
            case ViewDropdownOptions::ALL:
                disableMin(); disableMax();
                minCoordInput->setValue("0");
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
                minCoordInput->setValue("0");
                clampCoordInput(maxCoordInput);
                break;
            case ViewDropdownOptions::ALL_ABOVE:
                enableMin(); disableMax();
                clampCoordInput(minCoordInput);
                maxCoordInput->setValue(std::to_string(maxCurrentCoord()));
                break;
        }
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
    auto validateCoord = [](const std::string &s) -> bool {
        if (!s.length()) return false;
        auto val = std::stoi(s);
        return 0 <= val && 255 >= val;
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
                    // TODO: update some static value store or renderer view settings or something
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

    // Default values
    minCoordInput->setValue("0");
    maxCoordInput->setValue(std::to_string(XRES));
    disableMin();
    disableMax();

    addChild((new TextButton(
        Vector2{ PAD, size.y - styles::SETTINGS_BUTTON_HEIGHT - PAD },
        Vector2{ size.x - 2 * PAD, styles::SETTINGS_BUTTON_HEIGHT },
        "Set here"
    ))->setClickCallback([this](unsigned int) {
        // TODO
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
