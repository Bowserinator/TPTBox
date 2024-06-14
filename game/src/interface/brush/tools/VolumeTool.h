#ifndef INTERFACE_BRUSH_TOOLS_VOLUMETOOL_H_
#define INTERFACE_BRUSH_TOOLS_VOLUMETOOL_H_

#include "BrushShapeTool.h"

#include <utility>

class VolumeBrushTool : public BrushShapeTool {
public:
    VolumeBrushTool(): BrushShapeTool("Cuboid", 2) {}

    void operation(CLICK_BRUSH_OP_PARAMS) override {
        Vector3T<int> start = points[0];
        Vector3T<int> end = points[1];

        if (end.x < start.x) std::swap(end.x, start.x);
        if (end.y < start.y) std::swap(end.y, start.y);
        if (end.z < start.z) std::swap(end.z, start.z);

        for (int x = start.x; x <= end.x; x++)
        for (int y = start.y; y <= end.y; y++)
        for (int z = start.z; z <= end.z; z++) {
            if (settings::data::ref()->graphics->in_view_slice(x, y, z))
                brush_renderer->apply_brush_op(x, y, z);
        }
    }
};

#endif // INTERFACE_BRUSH_TOOLS_VOLUMETOOL_H_
