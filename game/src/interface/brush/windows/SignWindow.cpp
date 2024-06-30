#include "SignWindow.h"
#include "../../../simulation/Simulation.h"
#include "../../gui/components/TextButton.h"
#include "../../../simulation/Sign.h"
#include "../BrushShapeToolNumbers.h"

#include <string>

using namespace ui;

SignWindow ::SignWindow(const Vector2 &pos, const Vector2 &size, Simulation * sim, const Vector3T<int> click_pos,
        Sign * sign_to_edit):
    ui::Window(pos, size, ui::Window::Settings {
        .bottomPadding = styles::SETTINGS_BUTTON_HEIGHT,
        .interceptEvents = true,
        .title = (sign_to_edit ? "Edit" : "Create") + std::string{" Sign"}
    }, Style::getDefault()), sim(sim)
{
    addChild((new TextButton(
            Vector2{ 0, size.y - styles::SETTINGS_BUTTON_HEIGHT },
            Vector2{ size.x / 2, styles::SETTINGS_BUTTON_HEIGHT },
            "Cancel"
        ))->setClickCallback([this](unsigned int _button) {
            tryClose(ui::Window::CloseReason::BUTTON);
        }));
    addChild((new TextButton(
            Vector2{ size.x / 2, size.y - styles::SETTINGS_BUTTON_HEIGHT },
            Vector2{ size.x / 2, styles::SETTINGS_BUTTON_HEIGHT },
            "OK"
        ))->setClickCallback([this, sim, click_pos, sign_to_edit](unsigned int _button) {
            if (textInput->value().length()) {
                if (!sign_to_edit)
                    sim->signs.add_sign(textInput->value(), click_pos.x, click_pos.y, click_pos.z);
                else
                    sign_to_edit->set_text(textInput->value());
            }
            tryClose(ui::Window::CloseReason::BUTTON);
        }));

    constexpr float PAD = 10.0f;
    textInput = (new ui::TextInput(Vector2{ PAD, PAD + m_settings.headerHeight }, Vector2{ size.x - 2 * PAD, 30.0f }))
        ->setMaxLength(40);
    if (sign_to_edit)
        textInput->setValue(sign_to_edit->text);
    addChild(textInput);

    moveButton = (new ui::TextButton(Vector2{ size.x - PAD - 150.0f, PAD * 2 + m_settings.headerHeight + 30.0f },
        Vector2{ 150.0f, 30.0f }, "Move" ));
    moveButton->setClickCallback([this, sim, sign_to_edit](unsigned int _btn) {
        ((SignMoveBrushTool*)BRUSH_TOOLS[BRUSH_TOOL_MOVE_SIGN])->set_sign_to_edit(sign_to_edit);
        brush_renderer.set_brush_shape_tool(BRUSH_TOOLS[BRUSH_TOOL_MOVE_SIGN]);
        tryClose(ui::Window::CloseReason::BUTTON);
    });
    addChild(moveButton);

    deleteButton = (new ui::TextButton(
        Vector2{ size.x - 2 * (PAD + 150.0f), PAD * 2 + m_settings.headerHeight + 30.0f },
        Vector2{ 150.0f, 30.0f }, "Delete" ));
    deleteButton->setClickCallback([this, sim, sign_to_edit](unsigned int _btn) {
        sim->signs.remove_sign(sign_to_edit);
        tryClose(ui::Window::CloseReason::BUTTON);
    });
    addChild(deleteButton);

    if (!sign_to_edit) {
        moveButton->disable();
        deleteButton->disable();
    }
}
