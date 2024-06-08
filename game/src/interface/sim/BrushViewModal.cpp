#include "BrushViewModal.h"

#include "../gui/components/Panel.h"
#include "../gui/components/Dropdown.h"
#include "../gui/components/Label.h"
#include "../gui/components/TextInput.h"

#include "../../render/Renderer.h"
#include "../brush/Brush.h"

BrushViewModal::BrushViewModal(const Vector2 &pos, const Vector2 &size,
        Renderer * renderer, BrushRenderer * brushRenderer):
    ui::Modal(pos, size, Style::getDefault())
{
    viewBtn = (new TextButton(
            Vector2{ 0, 0 },
            Vector2{ size.x / 2, styles::SETTINGS_BUTTON_HEIGHT },
            "Slice View"
        ))->setClickCallback([this](unsigned int) {
            switchToView();
        });
    brushBtn = (new TextButton(
            Vector2{ size.x / 2.0f, 0 },
            Vector2{ size.x / 2, styles::SETTINGS_BUTTON_HEIGHT },
            "Brush"
        ))->setClickCallback([this](unsigned int) {
            switchToBrush();
        });
    addChild(viewBtn);
    addChild(brushBtn);

    viewPanel = new ViewPanel(Vector2{ 1, styles::SETTINGS_BUTTON_HEIGHT },
        size - Vector2{ 2, 2 + styles::SETTINGS_BUTTON_HEIGHT }, renderer);
    brushPanel = new BrushPanel(Vector2{ 1, styles::SETTINGS_BUTTON_HEIGHT },
        size - Vector2{ 2, 2 + styles::SETTINGS_BUTTON_HEIGHT }, brushRenderer);

    addChild(viewPanel);
    addChild(brushPanel);
    switchToView();
}

void BrushViewModal::switchToView() {
    if (currentPanel == CurrentPanel::View) return;
    viewPanel->pos.x = 1.0f;
    brushPanel->pos.x = -1000000;
    currentPanel = CurrentPanel::View;
}

void BrushViewModal::switchToBrush() {
    if (currentPanel == CurrentPanel::Brush) return;
    brushPanel->pos.x = 1.0f;
    viewPanel->pos.x = -1000000;
    currentPanel = CurrentPanel::Brush;
}
