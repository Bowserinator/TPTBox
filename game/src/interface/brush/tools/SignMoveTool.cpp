#include "../BrushShapeToolNumbers.h"
#include "SignMoveTool.h"

void SignMoveBrushTool::operation(CLICK_BRUSH_OP_PARAMS) {
    const auto &start = points[0].loc;

    if (!sign_to_edit) return;
    sign_to_edit->x = start.x;
    sign_to_edit->y = start.y;
    sign_to_edit->z = start.z;
    brush_renderer->set_brush_shape_tool(BRUSH_TOOLS[BRUSH_TOOL_SIGN]);
}

void SignMoveBrushTool::draw(CLICK_BRUSH_DRAW_PARAMS) {
    sim.signs.draw_single_sign(renderer, sign_to_edit, (Vector3)pos, GRAY);
}

void SignMoveBrushTool::onCancel(CANCEL_BRUSH_PARAMS) {
    brush_renderer->set_brush_shape_tool(BRUSH_TOOLS[BRUSH_TOOL_SIGN]);
}
