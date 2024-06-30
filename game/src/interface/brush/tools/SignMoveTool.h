#ifndef INTERFACE_BRUSH_TOOLS_SIGNMOVETOOL_H_
#define INTERFACE_BRUSH_TOOLS_SIGNMOVETOOL_H_

#include "BrushShapeTool.h"
#include "../../../globals.h"
#include "../../../simulation/Sign.h"
#include "../../../util/vector_op.h"

class SignMoveBrushTool : public BrushShapeTool {
public:
    SignMoveBrushTool(): BrushShapeTool("Move Sign", 1) {}

    void operation(CLICK_BRUSH_OP_PARAMS) override;
    void draw(CLICK_BRUSH_DRAW_PARAMS) override;
    void onCancel(CANCEL_BRUSH_PARAMS) override;
    void remesh(CLICK_BRUSH_REMESH_PARAMS) override { /* Do nothing */ }

    void set_sign_to_edit(Sign * sign) { this->sign_to_edit = sign; }

private:
    Sign * sign_to_edit = nullptr;
};

#endif // INTERFACE_BRUSH_TOOLS_SIGNMOVETOOL_H_
