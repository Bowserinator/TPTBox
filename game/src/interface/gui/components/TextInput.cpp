#include "TextInput.h"
#include "../../EventConsumer.h"
#include "../../../util/math.h"
#include "../../../util/str_format.h"

#include <algorithm>
#include <utility>

using namespace ui;

constexpr float PAD = 5.0f;

void TextInput::draw(const Vector2 &screenPos) {
    InteractiveComponent::draw(screenPos);

    DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, style.getBackgroundColor(this));
    DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, size.x, size.y },
        style.borderThickness,
        !m_input_valid ? RED : style.getBorderColor(this));

    SetTextLineSpacing(FONT_SIZE);
    BeginScissorMode(screenPos.x, screenPos.y, size.x - PAD, size.y);

    // Draw selection
    if (_hasSelection())
        DrawRectangle(screenPos.x + m_selectionX[0] + PAD - m_scrollX, screenPos.y + PAD,
            m_selectionX[1] - m_selectionX[0], size.y - 2 * PAD, Color{ 33, 137, 222, 255 });

    // Draw text
    if (m_value.length())
        DrawTextEx(FontCache::ref()->main_font, m_value.c_str(),
            screenPos + Vector2(PAD - m_scrollX, size.y / 2 - FONT_SIZE / 2),
            FONT_SIZE, FONT_SPACING,
            !m_input_valid ? RED : style.getTextColor(this));
    else if (m_config.placeholder.length())
        DrawTextEx(FontCache::ref()->main_font, m_config.placeholder.c_str(),
            screenPos + Vector2(PAD - m_scrollX, size.y / 2 - FONT_SIZE / 2),
            FONT_SIZE, FONT_SPACING,
            style.disabledTextColor);

    // Draw cursor (with blink)
    if (m_focused && static_cast<int>(GetTime() / 0.5) & 1)
        DrawRectangle(screenPos.x + m_cursorX - m_scrollX + PAD, screenPos.y + PAD, 2, size.y - 2 * PAD, WHITE);
    EndScissorMode();
}

void TextInput::tick(float dt) {
    InteractiveComponent::tick(dt);

    // Don't do anything if not focused
    if ((EventConsumer::ref()->isMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
         EventConsumer::ref()->isMouseButtonPressed(MOUSE_BUTTON_RIGHT)) && !contains(GetMousePosition() - globalPos)
    ) {
        unfocus();
        _deselect();
    }
    if (!m_focused) return;
    m_value_modified_in_tick = false;

    // Collect input:
    auto oldCursor = m_cursor;
    const bool ctrl = EventConsumer::ref()->isKeyDown(KEY_LEFT_CONTROL);
    const bool shift = EventConsumer::ref()->isKeyDown(KEY_LEFT_SHIFT);

    // Tab / Shift + Tab
    if (EventConsumer::ref()->isKeyPressed(KEY_TAB)) {
        if (shift && prevTabInput) {
            unfocus(); _deselect();
            prevTabInput->focus();
            EventConsumer::ref()->consumeKeyboard();
            return;
        } else if (!shift && nextTabInput) {
            unfocus(); _deselect();
            nextTabInput->focus();
            EventConsumer::ref()->consumeKeyboard();
            return;
        }
    }

    // Ctrl-A (select all)
    if (ctrl && m_value.length() && EventConsumer::ref()->isKeyPressed(KEY_A)) {
        m_selection[0] = 0;
        m_selection[1] = m_value.length();
        m_selectionX[0] = 0.0f;
        m_selectionX[1] = MeasureTextEx(FontCache::ref()->main_font, m_value.c_str(), FONT_SIZE, FONT_SPACING).x;
    }

    // Cut / Copy selection
    if (_hasSelection() && ctrl) {
        if (EventConsumer::ref()->isKeyPressed(KEY_X) && !isReadOnly()) {
            SetClipboardText(m_value.substr(m_selection[0], m_selection[1] - m_selection[0] + 1).c_str());
            _deselectAndDeleteSelection();
        } else if (EventConsumer::ref()->isKeyPressed(KEY_C)) {
            SetClipboardText(m_value.substr(m_selection[0], m_selection[1] - m_selection[0] + 1).c_str());
        }
    }

    // Paste
    if (ctrl && EventConsumer::ref()->isKeyPressed(KEY_V) && !isReadOnly()) {
        _deselectAndDeleteSelection();
        std::string tmp = GetClipboardText();
        tmp.erase(
            std::remove_if(tmp.begin(), tmp.end(), [this](char c)
                { return !m_input_allowed(std::string(1, c)); }),
            tmp.end());

        tmp = tmp.substr(0, m_config.maxLength);
        if (tmp.length()) {
            m_cursor = std::min(m_cursor, m_value.length());
            m_value = m_value.substr(0, m_cursor) + tmp + m_value.substr(m_cursor);
            m_value = m_value.substr(0, m_config.maxLength);
            m_cursor += tmp.length();
            m_value_modified_in_tick = true;
        }
    }

    // Insertion
    int in;
    while ((in = GetCharPressed()) && !isReadOnly()) {
        _deselectAndDeleteSelection();
        std::string inStr = util::unicode_to_utf8(in);
        if (m_value.length() < m_config.maxLength && m_input_allowed(inStr)) {
            m_cursor = std::min(m_cursor, m_value.length());
            if (m_cursor == m_value.length())
                m_value += inStr;
            else
                m_value = m_value.substr(0, m_cursor) + inStr + m_value.substr(m_cursor);
            m_cursor++;
            m_value_modified_in_tick = true;
        }
    }

    // Deletion
    if (m_cursor > 0 && m_value.length() && EventConsumer::ref()->isKeyPressedAny(KEY_BACKSPACE) && !isReadOnly()) {
        m_value_modified_in_tick = true;
        if (_hasSelection())
            _deselectAndDeleteSelection();
        else {
            if (m_cursor == m_value.length())
                m_value.pop_back();
            else
                m_value = m_value.substr(0, m_cursor - 1) + m_value.substr(m_cursor);
            m_cursor--;
        }
    }
    if (m_cursor < m_value.length() && EventConsumer::ref()->isKeyPressedAny(KEY_DELETE) && !isReadOnly()) {
        m_value_modified_in_tick = true;
        if (_hasSelection())
            _deselectAndDeleteSelection();
        else
            m_value = m_value.substr(0, m_cursor) + m_value.substr(m_cursor + 1);
    }

    // Move cursor
    if (!ctrl && EventConsumer::ref()->isKeyPressedAny(KEY_LEFT))
        m_cursor--;
    else if (!ctrl && EventConsumer::ref()->isKeyPressedAny(KEY_RIGHT))
        m_cursor++;
    else if (ctrl && EventConsumer::ref()->isKeyPressedAny(KEY_LEFT)) {
        m_cursor = std::min(m_cursor - 1, m_value.length());
        while (m_cursor > 0 && m_value[m_cursor] != ' ')
            m_cursor--;
    }
    else if (ctrl && EventConsumer::ref()->isKeyPressedAny(KEY_RIGHT)) {
        m_cursor = std::min(m_cursor + 1, m_value.length());
        while (m_cursor < m_value.length() && m_value[m_cursor] != ' ')
            m_cursor++;
    }

    _updateCursor(m_cursor, oldCursor);
    if (m_value_modified_in_tick) {
        m_input_valid = m_input_validation(m_value);
        m_on_value_change(m_value);
    }

    EventConsumer::ref()->consumeKeyboard();
}

void TextInput::onMouseClick(Vector2 localPos, unsigned button) {
    InteractiveComponent::onMouseClick(localPos, button);
    if (!m_disabled) focus();

    if (button == MOUSE_BUTTON_LEFT && m_value.length()) {
        const auto oldCursor = m_cursor;
        const auto bestCursorPos = _getCursorAtClick(localPos.x).cursor;
        _updateCursor(bestCursorPos, oldCursor);
    }
}

void TextInput::onMouseMoved(Vector2 localPos) {
    InteractiveComponent::onMouseMoved(localPos);
    if (m_disabled || !m_focused) return;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        _dragSelectionUpdate(localPos);
}

void TextInput::onMouseRelease(Vector2 localPos, unsigned button) {
    InteractiveComponent::onMouseRelease(localPos, button);

    if (m_disabled || !m_focused || button != MOUSE_BUTTON_LEFT) return;
    if (!m_value.length()) {
        _deselect();
        return;
    }
    _dragSelectionUpdate(localPos);
}

TextInput::CursorClick TextInput::_getCursorAtClick(const float localPosX) const {
    // Note: this *could* be optimized with binary search but
    // it also only takes like 0.2 us per character so...
    const float clickX = localPosX + m_scrollX;
    float bestError = std::abs(localPosX);
    float bestX = 0.0f;
    std::size_t bestCursorPos = 0;
    float runningX = 0.0f;
    char singleChar[] = { ' ', '\0' };

    for (std::size_t i = 1; i <= m_value.length(); i++) {
        singleChar[0] = m_value[i - 1];
        runningX += MeasureTextEx(FontCache::ref()->main_font, singleChar, FONT_SIZE, FONT_SPACING).x;
        if (std::abs(runningX - clickX) < bestError) {
            bestError = std::abs(runningX - clickX);
            bestCursorPos = i;
            bestX = runningX;
        }
    }
    return { bestCursorPos, bestX };
}

void TextInput::_updateCursor(std::size_t newVal, std::size_t oldCursor) {
    m_cursor = util::clamp(newVal, 0, m_value.length());
    if (m_cursor != oldCursor) {
        m_cursorX = MeasureTextEx(FontCache::ref()->main_font,
            m_value.substr(0, m_cursor).c_str(), FONT_SIZE, FONT_SPACING).x;
        m_scrollX = std::max(0.0f, m_cursorX - size.x + PAD * 2);
    }
}

void TextInput::_deselect() {
    m_selection[0] = 0;
    m_selection[1] = 0;
    m_selectionX[0] = -1.0f;
    m_selectionX[1] = -1.0f;
}

void TextInput::_deselectAndDeleteSelection() {
    if (!_hasSelection()) return;
    m_value = m_value.substr(0, m_selection[0]) + m_value.substr(m_selection[1]);
    m_value_modified_in_tick = true;
    if (m_cursor == m_selection[1]) // Move to selection[0]
        _updateCursor(std::min(m_selection[0], m_value.length()), m_cursor);
    _deselect();
}

void TextInput::_dragSelectionUpdate(const Vector2 localPos) {
    const auto bestCursorPosData = _getCursorAtClick(localPos.x);
    if (m_cursor == bestCursorPosData.cursor) {
        _deselect();
        return;
    }

    std::size_t selection1 = m_cursor;
    std::size_t selection2 = bestCursorPosData.cursor;
    float x1 = m_cursorX;
    float x2 =  bestCursorPosData.xOffset;

    if (selection1 > selection2) {
        std::swap(selection1, selection2);
        std::swap(x1, x2);
    }

    m_selection[0] = selection1;
    m_selection[1] = selection2;
    m_selectionX[0] = x1;
    m_selectionX[1] = x2;
}
