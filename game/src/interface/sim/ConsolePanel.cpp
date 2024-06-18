#include "ConsolePanel.h"

constexpr float LINE_HEIGHT = 22.0f;

ConsolePanel::ConsolePanel(const Vector2 &pos, const Vector2 &size, const Style &style):
        Panel(pos, size, Style(style).setAllBackgroundColors(BLANK)) {
    constexpr float SCROLL_PANEL_HEIGHT = 500.0f;
    input = new ui::TextInput(
        Vector2{ -1.0f, SCROLL_PANEL_HEIGHT + 5.0f },
        Vector2{ (float)GetScreenWidth() + 2.0f, 30.0f }
    );
    input->setOnSubmit([this](const std::string &val) {
        if (command_running || !val.length()) return;
        submitLine("> " + val);
        input->setValue("");

        // TODO: run command
        submitLine("example cmd output");
        command_running = false;
    });
    addChild(input);

    panel = new ui::ScrollPanel(
        Vector2{ 0.0f, 0.0f },
        Vector2{ (float)GetScreenWidth(), SCROLL_PANEL_HEIGHT }
    );
    addChild(panel);
}

void ConsolePanel::tick(float dt) {
    Panel::tick(dt);

    if (IsWindowResized()) {
        panel->size.x = size.x = (float)GetScreenWidth();
        input->size.x = size.x + 2.0f;
        size.y = (float)GetScreenHeight();
    }
}

void ConsolePanel::draw(const Vector2 &screenPos) {
    DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, Color{ 0, 0, 0, 200 });
    Panel::draw(screenPos);
}

void ConsolePanel::submitLine(const std::string &line) {
    panel->addChild(new ui::Label(
        Vector2{0, line_count * LINE_HEIGHT},
        Vector2{(float)GetScreenWidth(), LINE_HEIGHT},
        line
    ));
    line_count++;
    panel->scrollTo(9999999999.0f);
}
