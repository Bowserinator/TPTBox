#ifndef GRAPHICS_SETTING_MODAL_H
#define GRAPHICS_SETTING_MODAL_H

#include "../gui/components/Window.h"
#include "../gui/Style.h"

class GraphicsSettingsModal : public ui::Window {
public:
    GraphicsSettingsModal(const Vector2 &pos, const Vector2 &size);
};

/*
// TODO: take instance of Renderer?
// [OK] [Cancel]
// + reset

Dropdown:
Debug ray traces
debug normal vectors
debug ao
regular

Fullscreen / scaling?
*/

#endif