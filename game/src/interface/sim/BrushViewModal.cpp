#include "BrushViewModal.h"

#include "../gui/components/Panel.h"
#include "../gui/components/Dropdown.h"
#include "../gui/components/Label.h"
#include "../gui/components/TextInput.h"

#include "../../render/Renderer.h"
#include "./mini_panels/ViewPanel.h"

BrushViewModal::BrushViewModal(const Vector2 &pos, const Vector2 &size, Renderer * renderer):
    ui::Modal(pos, size, Style::getDefault())
{
    viewBtn = (new TextButton(
            Vector2{ 0, 0 },
            Vector2{ size.x / 2, styles::SETTINGS_BUTTON_HEIGHT },
            "Slice View"
        ))->setClickCallback([this](unsigned int) {
           
        });
    brushBtn = (new TextButton(
            Vector2{ size.x / 2.0f, 0 },
            Vector2{ size.x / 2, styles::SETTINGS_BUTTON_HEIGHT },
            "Brush"
        ))->setClickCallback([this](unsigned int) {
            
        });
    addChild(viewBtn);
    addChild(brushBtn);

    addChild(new ViewPanel(Vector2{ 1, styles::SETTINGS_BUTTON_HEIGHT }, size - Vector2{ 2, 2 + styles::SETTINGS_BUTTON_HEIGHT }, renderer));
}
