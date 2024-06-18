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

    void draw(CLICK_BRUSH_DRAW_PARAMS) {
        auto centerOfModel = points.size() ?
            (Vector3)(pos + points[0])  / 2.0f :
            (Vector3)pos;
        brush_preview::setup_shader(centerOfModel, renderer);

        brush_preview::draw_mesh_faces_on_render_tex(renderer, brush_preview::brush_tool_render_tex,
            preview_models, centerOfModel, false);
        util::draw_render_texture(brush_preview::brush_tool_render_tex);
    }

    void remesh(CLICK_BRUSH_REMESH_PARAMS) {
        unload_models();
        if (!points.size()) {
            if (!IsModelReady(preview_models[0]))
                preview_models = brush_preview::generate_cuboid_faces(1, 1, 1);
        } else {
            preview_models = brush_preview::generate_cuboid_faces(
                std::abs(pos.x - points[0].x) + 1,
                std::abs(pos.y - points[0].y) + 1,
                std::abs(pos.z - points[0].z) + 1
            );
        }
    }
};

#endif // INTERFACE_BRUSH_TOOLS_VOLUMETOOL_H_
