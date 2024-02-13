#include "ElementMenu.h"
#include "ElementMenuButton.h"

#include "../../simulation/SimulationDef.h"
#include "../../simulation/ElementClasses.h"

ElementMenu::ElementMenu(BrushRenderer * brush_renderer):
    brush_renderer(brush_renderer) {}

void ElementMenu::init() {
    ui::Panel * main_panel = new ui::Panel(
        Vector2{ 0, (float)GetScreenHeight() - 100 },
        Vector2{ (float)GetScreenWidth(), 100 }
    );

    for (auto id = 1; id <= ELEMENT_COUNT; id++) {
        const auto &el = GetElements()[id];
        Color bg_color = el.Color.as_Color();
        bg_color.a = 255;

        main_panel->addChild(new ElementMenuButton(
            Vector2{
                main_panel->size.x - (id % 22) * (ELEMENT_BUTTON_SIZE.x + 5),
                id > 22 ? ELEMENT_BUTTON_SIZE.y + 5 : 0
            },
            el.Name,
            bg_color,
            el.Color.brightness() < 128 ? WHITE : BLACK,
            brush_renderer
        ));
    }

    addChild(main_panel);
}

