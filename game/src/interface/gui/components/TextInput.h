#ifndef GUI_TEXT_INPUT_H
#define GUI_TEXT_INPUT_H

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

        void setValue(const std::string &text) {
            this->value = text.substr(0, config.maxLength);
            onValueChange(value);
            inputValid = inputValidation(value);
        }
        std::string getValue() { return value; }
        bool isReadOnly() const { return config.readOnly; }
        bool isInputValid() const { return inputValid; }

        TextInput * setPlaceholder(const std::string &text) { this->config.placeholder = text; return this; }
        TextInput * setMaxLength(const std::size_t maxLength) { this->config.maxLength = maxLength; return this; }
        TextInput * setReadOnly(const bool readOnly) { this->config.readOnly = readOnly; return this; }
        TextInput * setOnValueChange(std::function<void(const std::string&)> onValueChange) { this->onValueChange = onValueChange; return this; }
        TextInput * setInputValidation(std::function<bool(const std::string&)> inputValidation) { this->inputValidation = inputValidation; return this; }
        TextInput * setInputAllowed(std::function<bool(const std::string&)> inputAllowed) { this->inputAllowed = inputAllowed; return this; }

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

    protected:
        struct CursorClick {
            std::size_t cursor;
            float xOffset;
        };

        void update_cursor(std::size_t newVal, std::size_t oldVal);
        CursorClick get_cursor_at_click(const float localPosX) const;
        void deselect();
        void deselect_and_delete_selection();
        void drag_selection_update(const Vector2 localPos);
        bool has_selection() const { return selectionX[0] >= 0.0f; }

        Config config;
        std::string value = "";
        std::size_t cursor = 0;
        bool inputValid = true;

        std::function<void(const std::string&)> onValueChange = [](const std::string &in){};
        std::function<bool(const std::string&)> inputValidation = [](const std::string &in) -> bool { return true; };
        std::function<bool(const std::string&)> inputAllowed = [](const std::string &in) -> bool { return true; };

        // Computed
        float cursorX = 0.0f;
        float scrollX = 0.0f;
        std::size_t selection[2] = { 0, 0 };
        float selectionX[2] = {-1.0f, -1.0f};
        bool valueModifiedInTick = false;
    };
}

#endif