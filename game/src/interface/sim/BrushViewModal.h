#ifndef SIM_BRUSHVIEW_MODAL_H
#define SIM_BRUSHVIEW_MODAL_H

#include "../gui/components/Modal.h"
#include "../gui/components/TextButton.h"

class BrushViewModal : public ui::Modal {
public:
    BrushViewModal(const Vector2 &pos, const Vector2 &size);
private:
    ui::Button * viewBtn = nullptr;
    ui::Button * brushBtn = nullptr;
};

#endif