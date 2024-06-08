#include "BrushPanel.h"
#include "../../brush/Brushes.h"
#include "../../brush/Brush.h"

#include <string>
#include <utility>

using namespace ui;

BrushPanel::BrushPanel(const Vector2 &pos, const Vector2 &size, BrushRenderer * brushRenderer):
        ui::Panel(pos, size, Style::getDefault()), brushRenderer(brushRenderer) {
    constexpr float PAD = 5.0f;
    float Y = PAD * 2;

    brushTypeDropdown = (new Dropdown(
            Vector2{ PAD, Y },
            Vector2{ size.x - 2 * PAD, styles::DROPDOWN_SIZE.y })
        );
    for (int i = 0; i < BRUSHES.size(); i++)
        brushTypeDropdown = brushTypeDropdown->addOption(BRUSHES[i].name, i);

    brushTypeDropdown
        ->setUpdateCallback([this, brushRenderer](int _choice) {
            brushRenderer->set_brush_type(_choice);
        })
        ->switchToOption(0);
    addChild(brushTypeDropdown);

    auto createInputGroup = [this, size](const std::string &label, float Y) {
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
                ->setMaxLength(3)->setPlaceholder("1")
                ->setInputAllowed([](const std::string &s) { return s.length() == 1 && isdigit(s[0]); })
                ->setInputValidation([](const std::string &s) { return s.length() > 0; })
                ->setOnValueChange([this](const std::string &val) {
                    updateSizeValues();
                });
        addChild(textInput);

        auto btn = (new TextButton(
            Vector2{ 2.0f * PAD + 40.0f + size.x * 0.6f, Y },
            Vector2{ size.x * 0.4f - 3 * PAD - 40.0f, styles::DROPDOWN_SIZE.y },
            "+-"
        ))->setClickCallback([this, textInput](unsigned int button) {
            if (textInput->isInputValid() && textInput->value().length()) {
                auto val = std::stoi(textInput->value());
                const int inc = IsKeyDown(KEY_LEFT_SHIFT) ? 5 : 1;
                if (button == MOUSE_BUTTON_LEFT) val += inc;
                else if (button == MOUSE_BUTTON_RIGHT) val -= inc;

                val = util::clamp(val, 1, 100000);
                textInput->setValue(std::to_string(val));
                updateSizeValues();
            }
        });
        addChild(btn);
        return textInput;
    };

    Y += styles::DROPDOWN_SIZE.y + PAD;
    sizeXInput = createInputGroup("Size X", Y);
    Y += styles::DROPDOWN_SIZE.y + PAD;
    sizeYInput = createInputGroup("Size Y", Y);
    Y += styles::DROPDOWN_SIZE.y + PAD;
    sizeZInput = createInputGroup("Size Z", Y);

    sizeXInput->setValue(std::to_string( brushRenderer->get_size().x ));
    sizeYInput->setValue(std::to_string( brushRenderer->get_size().y ));
    sizeZInput->setValue(std::to_string( brushRenderer->get_size().z ));

    brushRenderer->add_change_listener([this, brushRenderer]() {
        brushTypeDropdown->switchToOption((int)brushRenderer->get_brush_type());
        sizeXInput->setValue(std::to_string(brushRenderer->get_size().x), true);
        sizeYInput->setValue(std::to_string(brushRenderer->get_size().y), true);
        sizeZInput->setValue(std::to_string(brushRenderer->get_size().z), true);
    });
}

void BrushPanel::updateSizeValues() {
    auto newSize = brushRenderer->get_size();
    if (sizeXInput->isInputValid() && sizeXInput->value().length())
        newSize.x = std::stoi(sizeXInput->value());
    if (sizeYInput->isInputValid() && sizeYInput->value().length())
        newSize.y = std::stoi(sizeYInput->value());
    if (sizeZInput->isInputValid() && sizeZInput->value().length())
        newSize.z = std::stoi(sizeZInput->value());
    brushRenderer->set_size(newSize);
}
