#include "TextInput.h"
#include "../../EventConsumer.h"
#include "../../../util/math.h"
#include "../../../util/str_format.h"

using namespace ui;

constexpr float PAD = 5.0f;

void TextInput::draw(const Vector2 &screenPos) {
    InteractiveComponent::draw(screenPos);

    DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, style.getBackgroundColor(this));
    DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, size.x, size.y },
        style.borderThickness,
        !inputValid ? RED : style.getBorderColor(this));

    SetTextLineSpacing(FONT_SIZE);
    BeginScissorMode(screenPos.x, screenPos.y, size.x - PAD, size.y);
    
    // Draw selection
    if (has_selection())
        DrawRectangle(screenPos.x + selectionX[0] + PAD - scrollX, screenPos.y + PAD,
            selectionX[1] - selectionX[0], size.y - 2 * PAD, Color{ 33, 137, 222, 255 });

    // Draw text
    if (value.length())
        DrawTextEx(FontCache::ref()->main_font, value.c_str(),
            screenPos + Vector2(PAD - scrollX, size.y / 2 - FONT_SIZE / 2),
            FONT_SIZE, FONT_SPACING,
            !inputValid ? RED : style.getTextColor(this));
    else if (config.placeholder.length())
        DrawTextEx(FontCache::ref()->main_font, config.placeholder.c_str(),
            screenPos + Vector2(PAD - scrollX, size.y / 2 - FONT_SIZE / 2),
            FONT_SIZE, FONT_SPACING,
            style.disabledTextColor);

    // Draw cursor (with blink)
    if (focused && static_cast<int>(GetTime() / 0.5) & 1)
        DrawRectangle(screenPos.x + cursorX - scrollX + PAD, screenPos.y + PAD, 2, size.y - 2 * PAD, WHITE);
    EndScissorMode();
}

void TextInput::tick(float dt) {
    InteractiveComponent::tick(dt);

    // Don't do anything if not focused
    if ((EventConsumer::ref()->isMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
         EventConsumer::ref()->isMouseButtonPressed(MOUSE_BUTTON_RIGHT)) && !contains(GetMousePosition() - globalPos)
    ) {
        unfocus();
        deselect();
    }
    if (!focused) return;
    valueModifiedInTick = false;

    // Collect input:
    auto oldCursor = cursor;
    const bool ctrl = EventConsumer::ref()->isKeyDown(KEY_LEFT_CONTROL);

    // Ctrl-A (select all)
    if (ctrl && value.length() && EventConsumer::ref()->isKeyPressed(KEY_A)) {
        selection[0] = 0;
        selection[1] = value.length();
        selectionX[0] = 0.0f;
        selectionX[1] = MeasureTextEx(FontCache::ref()->main_font, value.c_str(), FONT_SIZE, FONT_SPACING).x;
    }

    // Cut / Copy selection
    if (has_selection() && ctrl) {
        if (EventConsumer::ref()->isKeyPressed(KEY_X) && !isReadOnly()) {
            SetClipboardText(value.substr(selection[0], selection[1] - selection[0] + 1).c_str());
            deselect_and_delete_selection();
        } else if (EventConsumer::ref()->isKeyPressed(KEY_C)) {
            SetClipboardText(value.substr(selection[0], selection[1] - selection[0] + 1).c_str());
        }
    }

    // Paste
    if (ctrl && EventConsumer::ref()->isKeyPressed(KEY_V) && !isReadOnly()) {
        deselect_and_delete_selection();
        std::string tmp = GetClipboardText();
        tmp.erase(
            std::remove_if(tmp.begin(), tmp.end(), [this](char c)
                { return !inputAllowed(std::string(1, c)); }),
            tmp.end());

        tmp = tmp.substr(0, config.maxLength);
        if (tmp.length()) {
            cursor = std::min(cursor, value.length());
            value = value.substr(0, cursor) + tmp + value.substr(cursor);
            value = value.substr(0, config.maxLength);
            cursor += tmp.length();
            valueModifiedInTick = true;
        }
    }

    // Insertion
    int in;
    while ((in = GetCharPressed()) && !isReadOnly()) {
        deselect_and_delete_selection();
        std::string inStr = util::unicode_to_utf8(in);
        if (value.length() < config.maxLength && inputAllowed(inStr)) {
            cursor = std::min(cursor, value.length());
            if (cursor == value.length())
                value += inStr;
            else
                value = value.substr(0, cursor) + inStr + value.substr(cursor);
            cursor++;
            valueModifiedInTick = true;
        }
    }

    // Deletion
    if (cursor > 0 && value.length() && EventConsumer::ref()->isKeyPressedAny(KEY_BACKSPACE) && !isReadOnly()) {
        valueModifiedInTick = true;
        if (has_selection())
            deselect_and_delete_selection();
        else {
            if (cursor == value.length())
                value.pop_back();
            else
                value = value.substr(0, cursor - 1) + value.substr(cursor);
            cursor--;
        }
    }
    if (cursor < value.length() && EventConsumer::ref()->isKeyPressedAny(KEY_DELETE) && !isReadOnly()) {
        valueModifiedInTick = true;
        if (has_selection())
            deselect_and_delete_selection();
        else
            value = value.substr(0, cursor) + value.substr(cursor + 1);
    }

    // Move cursor
    if (!ctrl && EventConsumer::ref()->isKeyPressedAny(KEY_LEFT))
        cursor--;
    else if (!ctrl && EventConsumer::ref()->isKeyPressedAny(KEY_RIGHT))
        cursor++;
    else if (ctrl && EventConsumer::ref()->isKeyPressedAny(KEY_LEFT)) {
        cursor = std::min(cursor - 1, value.length());
        while (cursor > 0 && value[cursor] != ' ')
            cursor--;
    }
    else if (ctrl && EventConsumer::ref()->isKeyPressedAny(KEY_RIGHT)) {
        cursor = std::min(cursor + 1, value.length());
        while (cursor < value.length() && value[cursor] != ' ')
            cursor++;
    }

    update_cursor(cursor, oldCursor);
    if (valueModifiedInTick) {
        inputValid = inputValidation(value);
        onValueChange(value);
    }
}

void TextInput::onMouseClick(Vector2 localPos, unsigned button) {
    InteractiveComponent::onMouseClick(localPos, button);
    if (!disabled) focus();

    if (button == MOUSE_BUTTON_LEFT && value.length()) {
        const auto oldCursor = cursor;
        const auto bestCursorPos = get_cursor_at_click(localPos.x).cursor;
        update_cursor(bestCursorPos, oldCursor);
    }
}

void TextInput::onMouseMoved(Vector2 localPos) {
    InteractiveComponent::onMouseMoved(localPos);
    if (disabled || !focused) return;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        drag_selection_update(localPos);
}

void TextInput::onMouseRelease(Vector2 localPos, unsigned button) {
    InteractiveComponent::onMouseRelease(localPos, button);

    if (disabled || !focused || button != MOUSE_BUTTON_LEFT) return;
    if (!value.length()) {
        deselect();
        return;
    }
    drag_selection_update(localPos);
}

TextInput::CursorClick TextInput::get_cursor_at_click(const float localPosX) const {
    // Note: this *could* be optimized with binary search but
    // it also only takes like 0.2 us per character so...
    const float clickX = localPosX + scrollX;
    float bestError = std::abs(localPosX);
    float bestX = 0.0f;
    std::size_t bestCursorPos = 0;
    float runningX = 0.0f;
    char singleChar[] = { ' ', '\0' };

    for (std::size_t i = 1; i <= value.length(); i++) {
        singleChar[0] = value[i - 1];
        runningX += MeasureTextEx(FontCache::ref()->main_font, singleChar, FONT_SIZE, FONT_SPACING).x;
        if (std::abs(runningX - clickX) < bestError) {
            bestError = std::abs(runningX - clickX);
            bestCursorPos = i;
            bestX = runningX;
        }
    }
    return { bestCursorPos, bestX };
}

void TextInput::update_cursor(std::size_t newVal, std::size_t oldCursor) {
    cursor = util::clamp(newVal, 0, value.length());
    if (cursor != oldCursor) {
        cursorX = MeasureTextEx(FontCache::ref()->main_font, value.substr(0, cursor).c_str(), FONT_SIZE, FONT_SPACING).x;
        scrollX = std::max(0.0f, cursorX - size.x + PAD * 2);
    }
}

void TextInput::deselect() {
    selection[0] = 0;
    selection[1] = 0;
    selectionX[0] = -1.0f;
    selectionX[1] = -1.0f;
}

void TextInput::deselect_and_delete_selection() {
    if (!has_selection()) return;
    value = value.substr(0, selection[0]) + value.substr(selection[1]);
    valueModifiedInTick = true;
    if (cursor == selection[1]) // Move to selection[0]
        update_cursor(std::min(selection[0], value.length()), cursor);
    deselect();
}

void TextInput::drag_selection_update(const Vector2 localPos) {
    const auto bestCursorPosData = get_cursor_at_click(localPos.x);
    if (cursor == bestCursorPosData.cursor) {
        deselect();
        return;
    }

    std::size_t selection1 = cursor;
    std::size_t selection2 = bestCursorPosData.cursor;
    float x1 = cursorX;
    float x2 =  bestCursorPosData.xOffset;

    if (selection1 > selection2) {
        std::swap(selection1, selection2);
        std::swap(x1, x2);
    }

    selection[0] = selection1;
    selection[1] = selection2;
    selectionX[0] = x1;
    selectionX[1] = x2;
}
