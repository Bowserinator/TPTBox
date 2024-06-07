#ifndef INTERFACE_GUI_COMPONENTS_DROPDOWN_H_
#define INTERFACE_GUI_COMPONENTS_DROPDOWN_H_

#include "raylib.h"
#include "../styles.h"
#include "../Style.h"
#include "./abstract/InteractiveComponent.h"
#include "../../FontCache.h"

#include <functional>
#include <string>
#include <vector>
#include <utility>

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

        int selected() const { return m_selected_option; }

        Dropdown * switchToOption(int option);
        Dropdown * switchToOption(const std::string &option);
        Dropdown * addOption(const std::string &name, int id);
        Dropdown * removeOption(int id);
        Dropdown * setOptions(const std::vector<std::pair<std::string, int>> &options);

        Dropdown * setUpdateCallback(const std::function<void (int)> &f) { m_update_callback = f; return this; }

    protected:
        friend DropdownModal;
        int m_selected_option = -1;
        std::size_t m_selected_option_index = -1;

        std::vector<std::pair<std::string, int>> m_options;
        std::function<void (int)> m_update_callback = [](int){};
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_DROPDOWN_H_
