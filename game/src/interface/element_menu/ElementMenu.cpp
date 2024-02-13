#include "ElementMenu.h"
#include "ElementMenuButton.h"

#include "../../simulation/SimulationDef.h"
#include "../../simulation/ElementClasses.h"

#include "../gui/components/Panel.h"
#include "../gui/components/Modal.h"
#include "../gui/components/TextButton.h"
#include "../gui/styles.h"

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

        main_panel->addChild(new ui::TextButton(
            Vector2{
                main_panel->size.x - (id % 22) * (ELEMENT_BUTTON_SIZE.x + 5),
                id > 22 ? ELEMENT_BUTTON_SIZE.y + 5 : 0
            },
            styles::ELEMENT_BUTTON_SIZE,
            el.Name,
            bg_color,
            el.Color.brightness() < 128 ? WHITE : BLACK,
            bg_color, RED
        ));
    }

    addChild(main_panel);

    int * j = new int;
    *j = 0;
    auto f = [j]() {
        (*j)++;
        std::cout << "CLick " << *j << "\n";
    };

    ui::Modal * modal = new ui::Modal(Vector2{300, 0}, Vector2{400, 400});
    modal->addChild((new ui::TextButton(Vector2{10, 10}, Vector2{100, 30}, "Button"))
        ->setClickCallback(f)
        ->enable());

    addChild(modal);
}

