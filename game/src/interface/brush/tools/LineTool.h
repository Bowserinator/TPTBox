#ifndef INTERFACE_BRUSH_TOOLS_LINETOOL_H_
#define INTERFACE_BRUSH_TOOLS_LINETOOL_H_

#include "BrushShapeTool.h"
#include "../../../util/line.h"
#include "../../../simulation/SimulationDef.h"
#include <vector>

class LineBrushTool : public BrushShapeTool {
public:
    LineBrushTool(): BrushShapeTool("Line", 2) {}

    void operation(CLICK_BRUSH_OP_PARAMS) override {
        auto &start = points[0].loc;
        auto &end = points[1].loc;

        util::line3D(start.x, start.y, start.z, end.x, end.y, end.z, [brush_renderer](int x, int y, int z) {
            brush_renderer->draw_brush_at(x, y, z);
        });
    }

    void draw(CLICK_BRUSH_DRAW_PARAMS) {
        auto centerOfModel = points.size() ?
            Vector3{0, 0, 0} :
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
            auto &start = pos;
            auto &end = points[0].loc;

            std::array<std::vector<float>, 3> vertices;
            auto add_vertex = [&](BrushFace face, float x, float y, float z) {
                vertices[(int)face].push_back(x);
                vertices[(int)face].push_back(y);
                vertices[(int)face].push_back(z);
            };

            util::line3D(start.x, start.y, start.z, end.x, end.y, end.z, [add_vertex](int x, int y, int z) {
                add_vertex(BrushFace::Z, x, y, z + 1);
                add_vertex(BrushFace::Z, x + 1, y, z + 1);
                add_vertex(BrushFace::Z, x, y + 1, z + 1);
                add_vertex(BrushFace::Z, x + 1, y, z + 1);
                add_vertex(BrushFace::Z, x + 1, y + 1, z + 1);
                add_vertex(BrushFace::Z, x, y + 1, z + 1);

                add_vertex(BrushFace::Z, x, y, z);
                add_vertex(BrushFace::Z, x, y + 1, z);
                add_vertex(BrushFace::Z, x + 1, y, z);
                add_vertex(BrushFace::Z, x + 1, y, z);
                add_vertex(BrushFace::Z, x, y + 1, z);
                add_vertex(BrushFace::Z, x + 1, y + 1, z);

                add_vertex(BrushFace::X, x + 1, y, z);
                add_vertex(BrushFace::X, x + 1, y + 1, z);
                add_vertex(BrushFace::X, x + 1, y, z + 1);
                add_vertex(BrushFace::X, x + 1, y + 1, z);
                add_vertex(BrushFace::X, x + 1, y + 1, z + 1);
                add_vertex(BrushFace::X, x + 1, y, z + 1);

                add_vertex(BrushFace::X, x, y, z);
                add_vertex(BrushFace::X, x, y, z + 1);
                add_vertex(BrushFace::X, x, y + 1, z);
                add_vertex(BrushFace::X, x, y + 1, z);
                add_vertex(BrushFace::X, x, y, z + 1);
                add_vertex(BrushFace::X, x, y + 1, z + 1);

                add_vertex(BrushFace::Y, x, y + 1, z);
                add_vertex(BrushFace::Y, x, y + 1, z + 1);
                add_vertex(BrushFace::Y, x + 1, y + 1, z);
                add_vertex(BrushFace::Y, x + 1, y + 1, z);
                add_vertex(BrushFace::Y, x, y + 1, z + 1);
                add_vertex(BrushFace::Y, x + 1, y + 1, z + 1);

                add_vertex(BrushFace::Y, x, y, z);
                add_vertex(BrushFace::Y, x + 1, y, z);
                add_vertex(BrushFace::Y, x, y, z + 1);
                add_vertex(BrushFace::Y, x + 1, y, z);
                add_vertex(BrushFace::Y, x + 1, y, z + 1);
                add_vertex(BrushFace::Y, x, y, z + 1);
            });

            for (int i = 0; i < 3; i++) {
                Mesh mesh = { 0 };

                mesh.triangleCount = vertices[i].size() / 9;
                mesh.vertexCount = mesh.triangleCount * 3;
                mesh.vertices = vertices[i].data();
                mesh.texcoords = nullptr;
                mesh.normals = nullptr;

                util::upload_mesh_vertices_only(&mesh, false);
                mesh.vertices = nullptr;
                preview_models[i] = LoadModelFromMesh(mesh);
                preview_models[i].materials[0].shader = brush_preview::model_shader;
            }
        }
    }
};

#endif // INTERFACE_BRUSH_TOOLS_LINETOOL_H_
