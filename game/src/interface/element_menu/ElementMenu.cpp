#include "ElementMenu.h"
#include "../../simulation/SimulationDef.h"
#include "../../simulation/ElementClasses.h"
#include "../brush/Brush.h"

#include "../gui/components/Panel.h"
#include "../gui/components/Modal.h"
#include "../gui/components/TextButton.h"
#include "../gui/components/Label.h"
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

        float btnX = main_panel->size.x - (id % 22) * (styles::ELEMENT_BUTTON_SIZE.x + 5);
        float btnY = id > 22 ? styles::ELEMENT_BUTTON_SIZE.y + 5 : 0;

        ui::TextButton * btn = new ui::TextButton(
            Vector2{btnX, btnY},
            styles::ELEMENT_BUTTON_SIZE,
            el.Name,
            (ui::Style {
                .borderColor = bg_color,
                .hoverBorderColor = RED,
                .focusBorderColor = RED,
                .disabledBorderColor = bg_color,
                .borderThickness = 2.0f
            })
                .setAllBackgroundColors(bg_color)
                .setAllTextColors(el.Color.brightness() < 128 ? WHITE : BLACK)
        );
        btn->setClickCallback([this, id]() { brush_renderer->set_selected_element(id); });
        main_panel->addChild(btn);
    }

    addChild(main_panel);


    // TODO
    int * j = new int;
    *j = 0;
    auto f = [j]() {
        (*j)++;
        std::cout << "CLick " << *j << "\n";
    };

    ui::Modal * modal = new ui::Modal(Vector2{300, 20}, Vector2{400, 400});
    modal->addChild((new ui::TextButton(Vector2{10, 10}, Vector2{100, 30}, "Button"))
        ->setClickCallback(f)
        ->enable());
    modal->addChild(new ui::Label(Vector2{150, 10}, Vector2{200, 30}, "Hello", ui::Style{ .horizontalAlign = ui::Style::Align::Left } ));
    modal->addChild(new ui::Label(Vector2{150, 50}, Vector2{200, 30}, "Hello", ui::Style{ .horizontalAlign = ui::Style::Align::Center } ));
    modal->addChild(new ui::Label(Vector2{150, 90}, Vector2{200, 30}, "Hello", ui::Style{ .horizontalAlign = ui::Style::Align::Right } ));

    addChild(modal);
}

