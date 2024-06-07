#ifndef INTERFACE_SIM_BRUSHVIEWMODAL_H_
#define INTERFACE_SIM_BRUSHVIEWMODAL_H_

#include "../gui/components/Modal.h"
#include "../gui/components/TextButton.h"

class Renderer;

class BrushViewModal : public ui::Modal {
public:
    BrushViewModal(const Vector2 &pos, const Vector2 &size, Renderer * renderer);
private:
    ui::Button * viewBtn = nullptr;
    ui::Button * brushBtn = nullptr;
};

#endif // INTERFACE_SIM_BRUSHVIEWMODAL_H_
