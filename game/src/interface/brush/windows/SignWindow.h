#ifndef INTERFACE_BRUSH_WINDOWS_SIGNWINDOW_H_
#define INTERFACE_BRUSH_WINDOWS_SIGNWINDOW_H_

#include "../../gui/components/Window.h"
#include "../../gui/components/TextInput.h"
#include "../../gui/components/TextButton.h"
#include "../../gui/Style.h"
#include "../../../util/vector_op.h"

class Sign;
class Simulation;
class SignWindow : public ui::Window {
public:
    SignWindow(const Vector2 &pos, const Vector2 &size, Simulation * sim, const Vector3T<int> click_pos,
        Sign * sign_to_edit = nullptr);

private:
    ui::TextInput * textInput = nullptr;
    ui::TextButton
        * deleteButton = nullptr,
        * moveButton = nullptr;

    Simulation * sim;
};

#endif // INTERFACE_BRUSH_WINDOWS_SIGNWINDOW_H_
