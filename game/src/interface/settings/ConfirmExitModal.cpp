#include "ConfirmExitModal.h"
#include "../gui/components/TextButton.h"
#include "../gui/components/Label.h"
#include "../gui/styles.h"

using namespace ui;

ConfirmExitModal::ConfirmExitModal(const Vector2 &pos, const Vector2 &size):
    ui::Window(pos, size, ui::Window::Settings {
        .bottomPadding = styles::SETTINGS_BUTTON_HEIGHT,
        .interceptEvents = true,
        .title = "Confirm Exit"
    }, Style::getDefault())
{
    addChild((new TextButton(
            Vector2{ 0, size.y - styles::SETTINGS_BUTTON_HEIGHT },
            Vector2{ size.x * 2 / 3, styles::SETTINGS_BUTTON_HEIGHT },
            "Cancel"
        ))->setClickCallback([this](unsigned int) {
            tryClose(ui::Window::CloseReason::BUTTON);
        }));

    addChild((new TextButton(
            Vector2{ size.x * 2 / 3, size.y - styles::SETTINGS_BUTTON_HEIGHT },
            Vector2{ size.x / 3, styles::SETTINGS_BUTTON_HEIGHT },
            "Confirm",
            Style::getDefault().setAllTextColors(YELLOW)
        ))->setClickCallback([](unsigned int) {
            EventConsumer::ref()->flagExit();
        }));

    addChild(new Label(
        Vector2{ 10.0f, 15.0f + settings.headerHeight },
        Vector2{ size.x, 20.0f },
        "Are you sure you want to quit the game?",
        Style { .horizontalAlign = Style::Align::Left }
    ));
}
