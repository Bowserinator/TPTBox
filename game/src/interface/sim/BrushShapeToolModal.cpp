#include "BrushShapeToolModal.h"

#include "../gui/components/Panel.h"
#include "../gui/components/Dropdown.h"
#include "../gui/components/Label.h"
#include "../gui/components/TextInput.h"
#include "../gui/Style.h"

#include "../../render/Renderer.h"
#include "../brush/Brush.h"
#include "../brush/BrushShapeToolNumbers.h"

using namespace ui;

BrushShapeToolModal::BrushShapeToolModal(const Vector2 &pos, const Vector2 &size,
        Renderer * renderer, BrushRenderer * brushRenderer):
    ui::Modal(pos, size, Style::getDefault())
{
    constexpr float PAD = 5.0f;
    defaultBrushBtn = new ui::IconButton(
        Vector2{ PAD, PAD },
        Vector2{ styles::SETTINGS_BUTTON_HEIGHT, styles::SETTINGS_BUTTON_HEIGHT },
        ICON_BRUSH);
    defaultBrushBtn->setClickCallback([this, brushRenderer](unsigned int) {
        brushRenderer->set_brush_shape_tool(BRUSH_TOOLS[BRUSH_TOOL_DEFAULT]);
        switch_btn(defaultBrushBtn);
    });

    volumeBrushBtn = new ui::IconButton(
        Vector2{ PAD + styles::SETTINGS_BUTTON_HEIGHT, PAD },
        Vector2{ styles::SETTINGS_BUTTON_HEIGHT, styles::SETTINGS_BUTTON_HEIGHT },
        ICON_VOLUME);
    volumeBrushBtn->setClickCallback([this, brushRenderer](unsigned int) {
        brushRenderer->set_brush_shape_tool(BRUSH_TOOLS[BRUSH_TOOL_VOLUME]);
        switch_btn(volumeBrushBtn);
    });

    fillBrushBtn = new ui::IconButton(
        Vector2{ PAD + 2 * styles::SETTINGS_BUTTON_HEIGHT, PAD },
        Vector2{ styles::SETTINGS_BUTTON_HEIGHT, styles::SETTINGS_BUTTON_HEIGHT },
        ICON_FILL);
    fillBrushBtn->setClickCallback([this, brushRenderer](unsigned int) {
        brushRenderer->set_brush_shape_tool(BRUSH_TOOLS[BRUSH_TOOL_FILL]);
        switch_btn(fillBrushBtn);
    });

    lineBrushBtn = new ui::IconButton(
        Vector2{ PAD + 3 * styles::SETTINGS_BUTTON_HEIGHT, PAD },
        Vector2{ styles::SETTINGS_BUTTON_HEIGHT, styles::SETTINGS_BUTTON_HEIGHT },
        ICON_LINE);
    lineBrushBtn->setClickCallback([this, brushRenderer](unsigned int) {
        brushRenderer->set_brush_shape_tool(BRUSH_TOOLS[BRUSH_TOOL_LINE]);
        switch_btn(lineBrushBtn);
    });

    signBrushBtn = new ui::IconButton(
        Vector2{ PAD + 4 * styles::SETTINGS_BUTTON_HEIGHT, PAD },
        Vector2{ styles::SETTINGS_BUTTON_HEIGHT, styles::SETTINGS_BUTTON_HEIGHT },
        ICON_SIGN);
    signBrushBtn->setClickCallback([this, brushRenderer](unsigned int) {
        brushRenderer->set_brush_shape_tool(BRUSH_TOOLS[BRUSH_TOOL_SIGN]);
        switch_btn(signBrushBtn);
    });

    btns.push_back(defaultBrushBtn);
    btns.push_back(volumeBrushBtn);
    btns.push_back(fillBrushBtn);
    btns.push_back(lineBrushBtn);
    btns.push_back(signBrushBtn);

    for (auto b : btns)
        addChild(b);
    switch_btn(defaultBrushBtn);
}

void BrushShapeToolModal::switch_btn(ui::IconButton * btn) {
    for (auto btnOther : btns) {
        if (btnOther != btn) {
            btnOther->style.setAllBackgroundColors(BLACK);
            btnOther->style.setAllTextColors(WHITE);
        } else {
            btnOther->style.setAllBackgroundColors(WHITE);
            btnOther->style.setAllTextColors(BLACK);
        }
    }
}
