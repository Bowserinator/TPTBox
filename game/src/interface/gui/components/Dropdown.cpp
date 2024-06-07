#include "Dropdown.h"
#include "Modal.h"
#include "TextButton.h"
#include "../Scene.h"

#include "../../EventConsumer.h"
#include "../../../util/vector_op.h"

using namespace ui;

// Modal for showing dropdown options - temporarily added
// to the parent modal
class DropdownModal : public Modal {
public:
    DropdownModal(
        const Vector2 &pos, const Vector2 &size,
        std::vector<std::pair<std::string, int>> &options,
        Dropdown * dropdown
    ):
            Modal(pos, size, Style::getDefault()), options(options), dropdown(dropdown) {
        float optHeight = size.y / options.size();

        for (std::size_t i = 0; i < options.size(); i++) {
            addChild((new TextButton(
                Vector2{ 0, i * optHeight },
                Vector2{ size.x, optHeight },
                options[i].first
            ))->setClickCallback([dropdown, &options, i](unsigned int){
                dropdown->switchToOption(options[i].second);
                dropdown->m_update_callback(options[i].second);
            }));
        }
    }

    void onMouseClick(Vector2 localPos, unsigned button) override {
        Modal::onMouseClick(localPos, button);
        removeFromParent(this);
    }

    void onUnfocus() override {
        Modal::onUnfocus();
        removeFromParent(this);
    }

private:
    std::vector<std::pair<std::string, int>> &options;
    Dropdown * dropdown = nullptr;
};



void Dropdown::draw(const Vector2 &screenPos) {
    DrawRectangle(screenPos.x, screenPos.y, size.x, size.y, style.getBackgroundColor(this));
    DrawRectangleLinesEx(Rectangle { screenPos.x, screenPos.y, size.x, size.y },
        style.borderThickness,
        style.getBorderColor(this));

    constexpr float TRIANGLE_SIZE = 14.0f;
    const Vector2 pad{ -10.0f, size.y / 2 - TRIANGLE_SIZE / 4 };
    DrawTriangle(
        Vector2{ screenPos.x + size.x, screenPos.y } + pad,
        Vector2{ screenPos.x + size.x - TRIANGLE_SIZE, screenPos.y } + pad,
        Vector2{ screenPos.x + size.x - TRIANGLE_SIZE / 2, screenPos.y + TRIANGLE_SIZE / 2 } + pad,
        style.getBorderColor(this));

    if (m_selected_option_index >= 0) {
        const auto text = m_options[m_selected_option_index].first;
        const auto tsize = MeasureTextEx(FontCache::ref()->main_font, text.c_str(), FONT_SIZE, FONT_SPACING);
        const Vector2 pad = style.align(size, Vector2{ 5, 5 }, tsize);

        SetTextLineSpacing(FONT_SIZE);
        DrawTextEx(FontCache::ref()->main_font, text.c_str(),
            Vector2{screenPos.x + pad.x, screenPos.y + pad.y},
            FONT_SIZE, FONT_SPACING,
            style.getTextColor(this));
    }
}

void Dropdown::onMouseClick(Vector2 localPos, unsigned button) {
    InteractiveComponent::onMouseClick(localPos, button);
    addToParent(new DropdownModal(pos, Vector2{size.x, m_options.size() * size.y}, m_options, this));
}

Dropdown * Dropdown::switchToOption(int option) {
    m_selected_option_index = -1;
    for (std::size_t i = 0; i < m_options.size(); i++)
        if (m_options[i].second == option) {
            m_selected_option = m_options[i].second;
            m_selected_option_index = i;
            break;
        }
    return this;
}

Dropdown * Dropdown::switchToOption(const std::string &option) {
    m_selected_option_index = -1;
    for (std::size_t i = 0; i < m_options.size(); i++)
        if (m_options[i].first == option) {
            m_selected_option = m_options[i].second;
            m_selected_option_index = i;
            break;
        }
    return this;
}

Dropdown * Dropdown::addOption(const std::string &name, int id) {
    m_options.push_back(std::make_pair(name, id));
    return this;
}

Dropdown * Dropdown::removeOption(int id) {
    m_options.erase(std::remove_if(m_options.begin(), m_options.end(),
        [id](auto x) { return x.second == id; }), m_options.end());
    return this;
}

Dropdown * Dropdown::setOptions(const std::vector<std::pair<std::string, int>> &options) {
    this->m_options = options;
    return switchToOption(options.size() ? -1 : this->m_options[0].second);
}
