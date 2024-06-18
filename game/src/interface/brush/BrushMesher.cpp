#include "BrushMesher.h"
#include "../../render/constants.h"
#include "../../render/Renderer.h"
#include "../../render/camera/camera.h"
#include "../../util/graphics.h"
#include "../../simulation/SimulationDef.h"
#include "Preview.h"

#include <vector>
#include <GLFW/glfw3.h>

BrushFaceModels BrushFaceModels::GenBrushModel(const Brush &brush, Vector3T<int> size) {
    if (!IsShaderReady(brush_model_shader)) {
        #ifdef EMBED_SHADERS
        #include "../../resources/shaders/generated/brush_clip.vs.h"
        #include "../../resources/shaders/generated/brush_clip.fs.h"
        brush_model_shader = LoadShaderFromMemory(brush_clip_vs_source, brush_clip_fs_source);
        #else
        brush_model_shader = LoadShader("resources/shaders/brush_clip.vs", "resources/shaders/brush_clip.fs");
        #endif

        brush_model_shader_mv_loc = GetShaderLocation(brush_model_shader, "mv");
        brush_model_shader_depth_tex_loc = GetShaderLocation(brush_model_shader, "depth");
        brush_model_shader_res_loc = GetShaderLocation(brush_model_shader, "resolution");

        util::set_shader_value(brush_model_shader, GetShaderLocation(brush_model_shader, "simRes"),
            Vector3{ (float)XRES, (float)YRES, (float)ZRES });
    }

    std::vector<unsigned char> voxel_fill_map(size.x * size.y * size.z, 0);

    auto idx = [size](int x, int y, int z) { return x + y * size.x + z * size.x * size.y; };
    auto is_filled = [size, idx, &voxel_fill_map](int x, int y, int z) {
        if (x < 0 || y < 0 || z < 0 || x >= size.x || y >= size.y || z >= size.z)
            return false;
        return voxel_fill_map[idx(x, y, z)] != 0;
    };

    #pragma omp parallel for
    for (int z = 0; z < size.z; z++)
    for (int y = 0; y < size.y; y++)
    for (int x = 0; x < size.x; x++) {
        Vector3T<int> subpos{ x - size.x / 2, y - size.y / 2, z - size.z / 2 };

        if (brush.map((Vector3)subpos, (Vector3)size, Vector3{0.0f, 0.0f, 0.0f}))
            voxel_fill_map[idx(x, y, z)] = 1;
    }

    BrushFaceModels out{
        .models = {},
        .size = size
    };
    std::array<std::vector<float>, 3> vertices;

    auto add_vertex = [&](BrushFace face, int x, int y, int z) {
        vertices[(int)face].push_back(x);
        vertices[(int)face].push_back(y);
        vertices[(int)face].push_back(z);
    };

    if (brush.solid) {
        #pragma omp parallel for
        for (int i = 0; i < 3; i++) {
            if (i == 0) {
                for (int y = 0; y < size.y; y++) { // Front / back
                for (int x = 0; x < size.x; x++) {
                    // Front faces
                    int z = size.z;
                    while (!is_filled(x, y, z) && z >= 0) { z--; }
                    if (z >= 0) {
                        z++;
                        add_vertex(BrushFace::Z, x, y, z);
                        add_vertex(BrushFace::Z, x + 1, y, z);
                        add_vertex(BrushFace::Z, x, y + 1, z);
                        add_vertex(BrushFace::Z, x + 1, y, z);
                        add_vertex(BrushFace::Z, x + 1, y + 1, z);
                        add_vertex(BrushFace::Z, x, y + 1, z);
                    }

                    // Back faces
                    z = 0;
                    while (!is_filled(x, y, z) && z <= size.z) { z++; }
                    if (z <= size.z) {
                        add_vertex(BrushFace::Z, x, y, z);
                        add_vertex(BrushFace::Z, x, y + 1, z);
                        add_vertex(BrushFace::Z, x + 1, y, z);
                        add_vertex(BrushFace::Z, x + 1, y, z);
                        add_vertex(BrushFace::Z, x, y + 1, z);
                        add_vertex(BrushFace::Z, x + 1, y + 1, z);
                    }
                }}
            }
            else if (i == 1) {
                for (int z = 0; z < size.z; z++) {
                for (int y = 0; y < size.y; y++) { // Left / right
                    // Left faces
                    int x = size.x;
                    while (!is_filled(x, y, z) && x >= 0) { x--; }
                    if (x >= 0) {
                        x++;
                        add_vertex(BrushFace::X, x, y, z);
                        add_vertex(BrushFace::X, x, y + 1, z);
                        add_vertex(BrushFace::X, x, y, z + 1);
                        add_vertex(BrushFace::X, x, y + 1, z);
                        add_vertex(BrushFace::X, x, y + 1, z + 1);
                        add_vertex(BrushFace::X, x, y, z + 1);
                    }

                    // Right faces
                    x = 0;
                    while (!is_filled(x, y, z) && x <= size.x) { x++; }
                    if (x <= size.x) {
                        add_vertex(BrushFace::X, x, y, z);
                        add_vertex(BrushFace::X, x, y, z + 1);
                        add_vertex(BrushFace::X, x, y + 1, z);
                        add_vertex(BrushFace::X, x, y + 1, z);
                        add_vertex(BrushFace::X, x, y, z + 1);
                        add_vertex(BrushFace::X, x, y + 1, z + 1);
                    }
                }}
            }
            else if (i == 2) {
                for (int z = 0; z < size.z; z++) { // Top / bottom
                for (int x = 0; x < size.x; x++) {
                    // Top faces
                    int y = size.y;
                    while (!is_filled(x, y, z) && y >= 0) { y--; }
                    if (y >= 0) {
                        y++;
                        add_vertex(BrushFace::Y, x, y, z);
                        add_vertex(BrushFace::Y, x, y, z + 1);
                        add_vertex(BrushFace::Y, x + 1, y, z);
                        add_vertex(BrushFace::Y, x + 1, y, z);
                        add_vertex(BrushFace::Y, x, y, z + 1);
                        add_vertex(BrushFace::Y, x + 1, y, z + 1);
                    }

                    // Bottom faces
                    y = 0;
                    while (!is_filled(x, y, z) && y <= size.y) { y++; }
                    if (y <= size.y) {
                        add_vertex(BrushFace::Y, x, y, z);
                        add_vertex(BrushFace::Y, x + 1, y, z);
                        add_vertex(BrushFace::Y, x, y, z + 1);
                        add_vertex(BrushFace::Y, x + 1, y, z);
                        add_vertex(BrushFace::Y, x + 1, y, z + 1);
                        add_vertex(BrushFace::Y, x, y, z + 1);
                    }
                }}
            }
        }
    }
    else {
        for (int z = 0; z <= size.z; z++) {
        for (int y = 0; y <= size.y; y++) {
        for (int x = 0; x <= size.x; x++) {
            // Top faces
            int x2 = x;
            if (!is_filled(x2, y, z) && is_filled(x2, y - 1, z))
                x2++;
            if (x2 > x) {
                add_vertex(BrushFace::Y, x, y, z);
                add_vertex(BrushFace::Y, x, y, z + 1);
                add_vertex(BrushFace::Y, x2, y, z);
                add_vertex(BrushFace::Y, x2, y, z);
                add_vertex(BrushFace::Y, x, y, z + 1);
                add_vertex(BrushFace::Y, x2, y, z + 1);
            }

            // Bottom faces
            x2 = x;
            if (is_filled(x2, y, z) && !is_filled(x2, y - 1, z))
                x2++;
            if (x2 > x) {
                add_vertex(BrushFace::Y, x, y, z);
                add_vertex(BrushFace::Y, x2, y, z);
                add_vertex(BrushFace::Y, x, y, z + 1);
                add_vertex(BrushFace::Y, x2, y, z);
                add_vertex(BrushFace::Y, x2, y, z + 1);
                add_vertex(BrushFace::Y, x, y, z + 1);
            }

            // Left faces
            int y2 = y;
            if (!is_filled(x, y2, z) && is_filled(x - 1, y2, z))
                y2++;
            if (y2 > y) {
                add_vertex(BrushFace::X, x, y, z);
                add_vertex(BrushFace::X, x, y2, z);
                add_vertex(BrushFace::X, x, y, z + 1);
                add_vertex(BrushFace::X, x, y2, z);
                add_vertex(BrushFace::X, x, y2, z + 1);
                add_vertex(BrushFace::X, x, y, z + 1);
            }

            // Right faces
            y2 = y;
            if (is_filled(x, y2, z) && !is_filled(x - 1, y2, z))
                y2++;
            if (y2 > y) {
                add_vertex(BrushFace::X, x, y, z);
                add_vertex(BrushFace::X, x, y, z + 1);
                add_vertex(BrushFace::X, x, y2, z);
                add_vertex(BrushFace::X, x, y2, z);
                add_vertex(BrushFace::X, x, y, z + 1);
                add_vertex(BrushFace::X, x, y2, z + 1);
            }

            // Front faces
            x2 = x;
            if (!is_filled(x2, y, z) && is_filled(x2, y, z - 1))
                x2++;
            if (x2 > x) {
                add_vertex(BrushFace::Z, x, y, z);
                add_vertex(BrushFace::Z, x2, y, z);
                add_vertex(BrushFace::Z, x, y + 1, z);
                add_vertex(BrushFace::Z, x2, y, z);
                add_vertex(BrushFace::Z, x2, y + 1, z);
                add_vertex(BrushFace::Z, x, y + 1, z);
            }

            // Back faces
            x2 = x;
            if (is_filled(x2, y, z) && !is_filled(x2, y, z - 1))
                x2++;
            if (x2 > x) {
                add_vertex(BrushFace::Z, x, y, z);
                add_vertex(BrushFace::Z, x, y + 1, z);
                add_vertex(BrushFace::Z, x2, y, z);
                add_vertex(BrushFace::Z, x2, y, z);
                add_vertex(BrushFace::Z, x, y + 1, z);
                add_vertex(BrushFace::Z, x2, y + 1, z);
            }
        }}}
    } // END non-solid

    for (int i = 0; i < 3; i++) {
        Mesh mesh = { 0 };

        mesh.triangleCount = vertices[i].size() / 9;
        mesh.vertexCount = mesh.triangleCount * 3;
        mesh.vertices = vertices[i].data();
        mesh.texcoords = nullptr;
        mesh.normals = nullptr;

        util::upload_mesh_vertices_only(&mesh, false);
        mesh.vertices = nullptr;
        out.models[i] = LoadModelFromMesh(mesh);
        out.models[i].materials[0].shader = brush_model_shader;
    }

    out.render_tex = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    return out;
}

void BrushFaceModels::draw(Vector3T<int> center, Renderer * renderer, bool deleteMode) {
    const Vector3 centerOfModel = Vector3{
        static_cast<float>(center.x - (int)size.x / 2),
        static_cast<float>(center.y - (int)size.y / 2),
        static_cast<float>(center.z - (int)size.z / 2)
    };

    SetShaderValueMatrix(brush_model_shader, brush_model_shader_mv_loc,
        MatrixTranslate(
            centerOfModel.x,
            centerOfModel.y,
            centerOfModel.z
        ));
    util::set_shader_value(brush_model_shader, brush_model_shader_res_loc,
        Vector2{ (float)GetScreenWidth(), (float)GetScreenHeight() });
    rlEnableShader(brush_model_shader.id);
    rlSetUniformSampler(brush_model_shader_depth_tex_loc, renderer->get_base_tex().depthTexture);
    rlDisableShader();

    // WARNING: there is a cursed bug. You MUST draw anything
    // with gl (ie a rectangle, cube, etc...) before the BeginTextureMode()
    // below or the depth texture passed to the shader will give incorrect values
    // (I have no idea why)
    // The below is the brush volume outline
    Vector3 boundingBoxOffset = Vector3{
        size.x % 2 == 1 ? 0.5f : 0.0f,
        size.y % 2 == 1 ? 0.5f : 0.0f,
        size.z % 2 == 1 ? 0.5f : 0.0f
    };
    DrawCubeWires((Vector3)center + boundingBoxOffset, size.x, size.y, size.z, GRAY);

    brush_preview::draw_mesh_faces_on_render_tex(renderer, render_tex, models, centerOfModel, deleteMode);
    util::draw_render_texture(render_tex);
}

void BrushFaceModels::unload() {
    UnloadRenderTexture(render_tex);
    for (int i = 0; i < models.size(); i++)
        UnloadModel(models[i]);
}
