#ifndef INTERFACE_BRUSH_TOOLS_LINETOOL_H_
#define INTERFACE_BRUSH_TOOLS_LINETOOL_H_

#include "BrushShapeTool.h"
#include "../../../util/line.h"
#include "../../../simulation/SimulationDef.h"

class LineBrushTool : public BrushShapeTool {
public:
    LineBrushTool(): BrushShapeTool("Line", 2) {}

    void operation(CLICK_BRUSH_OP_PARAMS) override {
        auto &start = points[0];
        auto &end = points[1];

        util::line3D(start.x, start.y, start.z, end.x, end.y, end.z, [brush_renderer](int x, int y, int z) {
            brush_renderer->draw_brush_at(x, y, z);
        });
    }
};

#endif // INTERFACE_BRUSH_TOOLS_LINETOOL_H_
