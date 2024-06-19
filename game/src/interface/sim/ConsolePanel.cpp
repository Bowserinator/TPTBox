#include "ConsolePanel.h"
#include "./console/CommandExecutor.h"
#include "../../util/colored_text.h"

#include <format>
#include <sstream>
#include <algorithm>

constexpr float LINE_HEIGHT = 22.0f;

ConsolePanel::ConsolePanel(Simulation * sim, const Vector2 &pos, const Vector2 &size, const Style &style):
        Panel(pos, size, Style(style).setAllBackgroundColors(BLANK)), sim(sim) {
    constexpr float SCROLL_PANEL_HEIGHT = 500.0f;
    input = new ui::TextInput(
        Vector2{ -1.0f, SCROLL_PANEL_HEIGHT + 5.0f },
        Vector2{ (float)GetScreenWidth() + 2.0f, 30.0f }
    );
    input->setPlaceholder("Type !help for a list of commands");
    input->setInputAllowed([](const std::string &v) { return v != "`"; });
    input->setOnSubmit([this](const std::string &_val) {
        if (command_running || !_val.length()) return;
        std::string val = _val;
        {
            auto ss = std::stringstream{val};
            std::string formatted_cmd = "> ";
            for (std::string token; std::getline(ss, token, ' ');) {
                if (token[0] == '!')
                    formatted_cmd += text_format::F_SKYBLUE;
                else if (token.end() == std::find_if(token.begin(), token.end(),
                        [](unsigned char c) { return !isdigit(c) && c != '-'; }))
                    formatted_cmd += text_format::F_GOLD;

                formatted_cmd += token;
                formatted_cmd += text_format::F_RESET;
                formatted_cmd += ' ';
            }
            submitLine(formatted_cmd);
        }
        cmd_history.push_back(val);
        history_cursor = cmd_history.size();
        input->setValue("");

        // Run command and submit result
        double start_time = GetTime();
        auto result = executeCommand(this->sim, val);
        double time_taken = GetTime() - start_time;

        auto ss = std::stringstream{result};
        bool first_line = true;
        for (std::string line; std::getline(ss, line, '\n');) {
            submitLine(line, first_line ? time_taken : -1.0); // Only show time on first line
            first_line = false;
        }
        submitDivider();

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
    // Cycle history
    if (EventConsumer::ref()->isKeyPressed(KEY_UP) && cmd_history.size()) {
        history_cursor = history_cursor == 0 ? 0 : history_cursor - 1;
        input->setValue(cmd_history[history_cursor]);
    } else if (EventConsumer::ref()->isKeyPressed(KEY_DOWN) && cmd_history.size()) {
        history_cursor = history_cursor >= cmd_history.size() - 1 ? history_cursor : history_cursor + 1;
        input->setValue(cmd_history[std::min(history_cursor, cmd_history.size() - 1)]);
    }

    // ------ Events after may be consumed by UI ---
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

void ConsolePanel::submitLine(const std::string &line, const double time_taken) {
    panel->addChild(new ui::RichLabel(
        Vector2{0, line_count * LINE_HEIGHT},
        Vector2{(float)GetScreenWidth(), LINE_HEIGHT},
        line
    ));

    if (time_taken >= 0) {
        panel->addChild(new ui::RichLabel(
            Vector2{0, line_count * LINE_HEIGHT},
            Vector2{1000.0f, LINE_HEIGHT},
            std::format("({:.2f}s)", time_taken),
            (Style { .horizontalAlign = Style::Align::Right }).setAllTextColors(GRAY)
        ));
    }

    line_count++;
    panel->scrollTo(9999999999.0f);
}

void ConsolePanel::submitDivider() {
    panel->addChild(new ui::HR(
        Vector2{0, line_count * LINE_HEIGHT},
        Vector2{100000.0f, 1.0f}
    ));
}
