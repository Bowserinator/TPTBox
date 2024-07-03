#ifndef INTERFACE_BRUSH_TOOLS_BUILDTOTOOL_H_
#define INTERFACE_BRUSH_TOOLS_BUILDTOTOOL_H_

#include "BrushShapeTool.h"
#include "../../../simulation/SimulationDef.h"
#include <vector>

class BuildToBrushTool : public BrushShapeTool {
public:
    BuildToBrushTool(): BrushShapeTool("Build to me", 2) {}

    void operation(CLICK_BRUSH_OP_PARAMS) override {
        auto &start = points[0].loc;
        auto end = get_end_from_start(points[0], points[1].loc);

        util::line3D(start.x, start.y, start.z, end.x, end.y, end.z, [brush_renderer](int x, int y, int z) {
            brush_renderer->draw_brush_at(x, y, z);
        });
    }

    void draw(CLICK_BRUSH_DRAW_PARAMS) {
        auto centerOfModel = points.size() ?
            ((Vector3)(points[0].loc) + (Vector3)(get_end_from_start(points[0], pos))) / 2.0f :
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
            auto &start = points[0].loc;
            auto end = get_end_from_start(points[0], pos);

            m_model_size = Vector3T<int>(
                std::abs(start.x - end.x) + 1,
                std::abs(start.y - end.y) + 1,
                std::abs(start.z - end.z) + 1);
            m_model_size.x *= start.x - end.x < 0 ? -1 : 1;
            m_model_size.y *= start.y - end.y < 0 ? -1 : 1;
            m_model_size.z *= start.z - end.z < 0 ? -1 : 1;
            preview_models = brush_preview::generate_cuboid_faces(
                std::abs(m_model_size.x), std::abs(m_model_size.y), std::abs(m_model_size.z));
        }
    }

private:
    Vector3T<int> m_model_size{0, 0, 0};

    Vector3T<int> get_end_from_start(const BrushRenderer::m_ClickData &start, const Vector3T<int> &pos) {
        auto end = start.loc;
        if ((start.faces & RayCast::FACE_X).any())
            end.x = pos.x;
        else if ((start.faces & RayCast::FACE_Y).any())
            end.y = pos.y;
        else if ((start.faces & RayCast::FACE_Z).any())
            end.z = pos.z;
        return end;
    }
};

#endif // INTERFACE_BRUSH_TOOLS_BUILDTOTOOL_H_
