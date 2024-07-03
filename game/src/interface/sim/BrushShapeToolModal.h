#ifndef INTERFACE_SIM_BRUSHSHAPETOOLMODAL_H_
#define INTERFACE_SIM_BRUSHSHAPETOOLMODAL_H_

#include "../gui/components/Modal.h"
#include "../gui/components/TextButton.h"
#include "../gui/components/IconButton.h"

#include <vector>

class Renderer;
class BrushRenderer;

class BrushShapeToolModal : public ui::Modal {
public:
    BrushShapeToolModal(const Vector2 &pos, const Vector2 &size, Renderer * renderer, BrushRenderer * brushRenderer);

private:
    ui::IconButton
        * defaultBrushBtn,
        * volumeBrushBtn,
        * fillBrushBtn,
        * lineBrushBtn,
        * buildToMeBrushBtn,
        * signBrushBtn;

    std::vector<ui::IconButton *> btns;

    void switch_btn(ui::IconButton * btn);
};

#endif // INTERFACE_SIM_BRUSHSHAPETOOLMODAL_H_
