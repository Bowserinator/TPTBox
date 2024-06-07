#ifndef INTERFACE_GUI_COMPONENTS_TEXTINPUT_H_
#define INTERFACE_GUI_COMPONENTS_TEXTINPUT_H_

#include "raylib.h"
#include "./abstract/InteractiveComponent.h"
#include "../styles.h"
#include "../Style.h"
#include "../../FontCache.h"
#include "../../../util/vector_op.h"

#include <string>
#include <functional>

namespace ui {
    class TextInput : public InteractiveComponent {
    public:
        TextInput(
            const Vector2 &pos,
            const Vector2 &size,
            const Style &style = Style::getDefault()
        ): InteractiveComponent(pos, size, style) {}

        virtual ~TextInput() = default;

        void setValue(const std::string &text, const bool noCallback = false) {
            this->m_value = text.substr(0, m_config.maxLength);
            m_input_valid = m_input_validation(m_value);
            if (!noCallback) m_on_value_change(m_value);
        }
        std::string value() { return m_value; }
        bool isReadOnly() const { return m_config.readOnly; }
        bool isInputValid() const { return m_input_valid; }

        TextInput * setPlaceholder(const std::string &text) { this->m_config.placeholder = text; return this; }
        TextInput * setMaxLength(const std::size_t maxLength) { this->m_config.maxLength = maxLength; return this; }
        TextInput * setReadOnly(const bool readOnly) { this->m_config.readOnly = readOnly; return this; }
        TextInput * setOnValueChange(std::function<void(const std::string&)> onValueChange) {
            this->m_on_value_change = onValueChange; return this; }
        TextInput * setInputValidation(std::function<bool(const std::string&)> inputValidation) {
            this->m_input_validation = inputValidation; return this; }
        TextInput * setInputAllowed(std::function<bool(const std::string&)> inputAllowed) {
            this->m_input_allowed = inputAllowed; return this; }

        void draw(const Vector2 &screenPos) override;
        void tick(float dt) override;
        void onMouseClick(Vector2 localPos, unsigned button) override;
        void onMouseRelease(Vector2 localPos, unsigned button) override;
        void onMouseMoved(Vector2 localPos) override;

        struct Config {
            std::string placeholder = "";
            std::size_t maxLength = 10000000;
            bool readOnly = false;
        };

        TextInput * prevTabInput = nullptr;
        TextInput * nextTabInput = nullptr;

    protected:
        struct CursorClick {
            std::size_t cursor;
            float xOffset;
        };

        void _updateCursor(std::size_t newVal, std::size_t oldVal);
        CursorClick _getCursorAtClick(const float localPosX) const;
        void _deselect();
        void _deselectAndDeleteSelection();
        void _dragSelectionUpdate(const Vector2 localPos);
        bool _hasSelection() const { return m_selectionX[0] >= 0.0f; }

        Config m_config;
        std::string m_value = "";
        std::size_t m_cursor = 0;
        bool m_input_valid = true;

        std::function<void(const std::string&)> m_on_value_change = [](const std::string &in){};
        std::function<bool(const std::string&)> m_input_validation = [](const std::string &in) -> bool { return true; };
        std::function<bool(const std::string&)> m_input_allowed = [](const std::string &in) -> bool { return true; };

        // Computed
        float m_cursorX = 0.0f;
        float m_scrollX = 0.0f;
        std::size_t m_selection[2] = { 0, 0 };
        float m_selectionX[2] = {-1.0f, -1.0f};
        bool m_value_modified_in_tick = false;
    };
} // namespace ui

#endif // INTERFACE_GUI_COMPONENTS_TEXTINPUT_H_
