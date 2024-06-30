#ifndef INTERFACE_SIM_WINDOWS_ERRORWINDOW_H_
#define INTERFACE_SIM_WINDOWS_ERRORWINDOW_H_

#include "../../gui/components/Window.h"
#include "../../gui/components/TextButton.h"
#include "../../gui/components/Label.h"
#include "../../gui/Style.h"
#include "../../../util/vector_op.h"

#include <string>

using namespace ui;

class ErrorWindow : public ui::Window {
public:
    ErrorWindow(const Vector2 &pos, const Vector2 &size, const std::string &msg):
            ui::Window(pos, size, ui::Window::Settings {
                .headerHeight = 30.0f,
                .bottomPadding = styles::SETTINGS_BUTTON_HEIGHT,
                .interceptEvents = true,
                .titleColor = RED,
                .title = "Error"
            }, Style::getDefault()) {
        addChild((new TextButton(
            Vector2{ 0, size.y - styles::SETTINGS_BUTTON_HEIGHT },
            Vector2{ size.x, styles::SETTINGS_BUTTON_HEIGHT },
            "OK"
        ))->setClickCallback([this](unsigned int _button) {
            tryClose(ui::Window::CloseReason::BUTTON);
        }));

        constexpr float PAD = 10.0f;
        addChild(new Label(
            Vector2{ PAD, PAD + m_settings.headerHeight },
            Vector2{ size.x - 2 * PAD, 30.0f },
            msg
        ));
    }
};

#endif // INTERFACE_SIM_WINDOWS_ERRORWINDOW_H_
