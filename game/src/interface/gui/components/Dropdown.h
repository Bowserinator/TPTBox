#ifndef GUI_DROPDOWN_H
#define GUI_DROPDOWN_H

#include "raylib.h"
#include "../styles.h"
#include "../Style.h"
#include "./abstract/InteractiveComponent.h"
#include "../../FontCache.h"

#include <functional>
#include <string>
#include <vector>

class DropdownModal;

namespace ui {
    class Dropdown: public InteractiveComponent {
    public:
        Dropdown(
            const Vector2 &pos,
            const Vector2 &size,
            const Style &style = Style::getDefault()
        ): InteractiveComponent(pos, size, style, MOUSE_CURSOR_POINTING_HAND) {}

        void draw(const Vector2 &screenPos) override;
        void onMouseClick(Vector2 localPos, unsigned button) override;
    
        Dropdown * switchToOption(int option);
        Dropdown * switchToOption(const std::string &option);
        Dropdown * addOption(const std::string &name, int id);
        Dropdown * removeOption(int id);
        Dropdown * setOptions(const std::vector<std::pair<std::string, int>> &options);

        Dropdown * setUpdateCallback(const std::function<void (int)> &f) { updateCallback = f; return this; }
    protected:
        friend DropdownModal;
        int selectedOption = -1;
        std::size_t selectedOptionIndex = -1;

        std::vector<std::pair<std::string, int>> options;
        std::function<void (int)> updateCallback = [](int){};
    };
}

#endif