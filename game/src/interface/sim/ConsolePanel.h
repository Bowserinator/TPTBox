#ifndef INTERFACE_SIM_CONSOLEPANEL_H_
#define INTERFACE_SIM_CONSOLEPANEL_H_

#include "raylib.h"
#include "../gui/styles.h"
#include "../gui/Style.h"
#include "../gui/components/Panel.h"
#include "../gui/components/RichLabel.h"
#include "../gui/components/TextInput.h"
#include "../gui/components/ScrollPanel.h"
#include "../gui/components/HR.h"
#include "../EventConsumer.h"

#include <string>
#include <vector>

using namespace ui;

class Simulation;

class ConsolePanel: public ui::Panel {
public:
    ConsolePanel(Simulation * sim, const Vector2 &pos, const Vector2 &size, const Style &style = Style::getDefault());

    void draw(const Vector2 &screenPos) override;
    void tick(float dt) override;

    void onMouseWheelInside(Vector2 localPos, float d) override {
        Panel::onMouseWheelInside(localPos, d);
        EventConsumer::ref()->consumeMouse();
    }

    ui::TextInput * input = nullptr;
    ui::ScrollPanel * panel = nullptr;

private:
    void submitLine(const std::string &line, const double time_taken = -1.0f);
    void submitDivider();

    Simulation * sim;
    unsigned int line_count = 0;
    bool command_running = false;

    std::vector<std::string> cmd_history;
    std::size_t history_cursor = 0;
};

#endif // INTERFACE_SIM_CONSOLEPANEL_H_
