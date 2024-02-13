#include "ElementMenu.h"
#include "ElementMenuButton.h"

#include "../../simulation/SimulationDef.h"
#include "../../simulation/ElementClasses.h"

ElementMenu::ElementMenu(BrushRenderer * brush_renderer):
    main_panel(Vector2{0.0f, 0.0f}, Vector2{0.0f, 0.0f}),
    brush_renderer(brush_renderer)
{

    // const Vector2 &pos, const Vector2 &size, Color color, Color text_color, BrushRenderer * brush_renderer


}

void ElementMenu::init() {
    main_panel.pos = Vector2{ 0, (float)GetScreenHeight() - 100 };
    main_panel.size = Vector2{ (float)GetScreenWidth(), 100 };

    for (auto id = 1; id <= ELEMENT_COUNT; id++) {
        const auto &el = GetElements()[id];
        Color bg_color = el.Color.as_Color();
        bg_color.a = 255;

        main_panel.addChild(new ElementMenuButton(
            Vector2{
                main_panel.size.x - (id % 22) * (ELEMENT_BUTTON_SIZE.x + 5),
                id > 22 ? ELEMENT_BUTTON_SIZE.y + 5 : 0
            },
            el.Name,
            bg_color,
            el.Color.brightness() < 128 ? WHITE : BLACK,
            brush_renderer
        ));
    }
}

void ElementMenu::draw() {
    main_panel.draw(main_panel.pos);
}

void ElementMenu::update() {
    main_panel.updateGlobalControls();
}
