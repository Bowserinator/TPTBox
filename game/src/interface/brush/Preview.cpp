#include "Preview.h"
#include "../../render/constants.h"
#include "../../render/Renderer.h"
#include "../../render/camera/camera.h"
#include "../../util/graphics.h"
#include "../../simulation/SimulationDef.h"

#include <vector>

void brush_preview::init() {
    #ifdef EMBED_SHADERS
    #include "../../resources/shaders/generated/brush_clip.fs.h"
    model_shader = LoadShaderFromMemory(nullptr, brush_clip_fs_source);
    #else
    model_shader = LoadShader(nullptr, "resources/shaders/brush_clip.fs");
    #endif

    model_shader_depth_tex_loc = GetShaderLocation(model_shader, "depth");
    model_shader_res_loc = GetShaderLocation(model_shader, "resolution");

    brush_tool_render_tex = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
}

void brush_preview::setup_shader(const Vector3 centerOfModel, Renderer * renderer) {
    util::set_shader_value(model_shader, model_shader_res_loc,
        Vector2{ (float)GetScreenWidth(), (float)GetScreenHeight() });
    rlEnableShader(model_shader.id);
    rlSetUniformSampler(model_shader_depth_tex_loc, renderer->get_base_tex().depthTexture);
    rlDisableShader();
}

std::array<Model, 3> brush_preview::generate_cuboid_faces(float width, float height, float length) {
    std::array<std::vector<float>, 3> vertices;
    auto add_vertex = [&](BrushFace face, float x, float y, float z) {
        vertices[(int)face].push_back(x);
        vertices[(int)face].push_back(y);
        vertices[(int)face].push_back(z);
    };

    add_vertex(BrushFace::Z, -width / 2, -height / 2, length / 2);
    add_vertex(BrushFace::Z, width / 2, -height / 2, length / 2);
    add_vertex(BrushFace::Z, -width / 2, height / 2, length / 2);
    add_vertex(BrushFace::Z, width / 2, -height / 2, length / 2);
    add_vertex(BrushFace::Z, width / 2, height / 2, length / 2);
    add_vertex(BrushFace::Z, -width / 2, height / 2, length / 2);

    add_vertex(BrushFace::Z, -width / 2, -height / 2, -length / 2);
    add_vertex(BrushFace::Z, -width / 2, height / 2, -length / 2);
    add_vertex(BrushFace::Z, width / 2, -height / 2, -length / 2);
    add_vertex(BrushFace::Z, width / 2, -height / 2, -length / 2);
    add_vertex(BrushFace::Z, -width / 2, height / 2, -length / 2);
    add_vertex(BrushFace::Z, width / 2, height / 2, -length / 2);

    add_vertex(BrushFace::X, width / 2, -height / 2, -length / 2);
    add_vertex(BrushFace::X, width / 2, height / 2, -length / 2);
    add_vertex(BrushFace::X, width / 2, -height / 2, length / 2);
    add_vertex(BrushFace::X, width / 2, height / 2, -length / 2);
    add_vertex(BrushFace::X, width / 2, height / 2, length / 2);
    add_vertex(BrushFace::X, width / 2, -height / 2, length / 2);

    add_vertex(BrushFace::X, -width / 2, -height / 2, -length / 2);
    add_vertex(BrushFace::X, -width / 2, -height / 2, length / 2);
    add_vertex(BrushFace::X, -width / 2, height / 2, -length / 2);
    add_vertex(BrushFace::X, -width / 2, height / 2, -length / 2);
    add_vertex(BrushFace::X, -width / 2, -height / 2, length / 2);
    add_vertex(BrushFace::X, -width / 2, height / 2, length / 2);

    add_vertex(BrushFace::Y, -width / 2, height / 2, -length / 2);
    add_vertex(BrushFace::Y, -width / 2, height / 2, length / 2);
    add_vertex(BrushFace::Y, width / 2, height / 2, -length / 2);
    add_vertex(BrushFace::Y, width / 2, height / 2, -length / 2);
    add_vertex(BrushFace::Y, -width / 2, height / 2, length / 2);
    add_vertex(BrushFace::Y, width / 2, height / 2, length / 2);

    add_vertex(BrushFace::Y, -width / 2, -height / 2, -length / 2);
    add_vertex(BrushFace::Y, width / 2, -height / 2, -length / 2);
    add_vertex(BrushFace::Y, -width / 2, -height / 2, length / 2);
    add_vertex(BrushFace::Y, width / 2, -height / 2, -length / 2);
    add_vertex(BrushFace::Y, width / 2, -height / 2, length / 2);
    add_vertex(BrushFace::Y, -width / 2, -height / 2, length / 2);

    std::array<Model, 3> models;
    for (int i = 0; i < 3; i++) {
        Mesh mesh = { 0 };

        mesh.triangleCount = vertices[i].size() / 9;
        mesh.vertexCount = mesh.triangleCount * 3;
        mesh.vertices = vertices[i].data();
        mesh.texcoords = nullptr;
        mesh.normals = nullptr;

        util::upload_mesh_vertices_only(&mesh, false);
        mesh.vertices = nullptr;
        models[i] = LoadModelFromMesh(mesh);
        models[i].materials[0].shader = brush_preview::model_shader;
    }
    return models;
}

void brush_preview::draw_mesh_faces(const std::array<Model, 3> &models, const Vector3 center_of_model,
        const bool delete_mode) {
    for (int i = 0; i < models.size(); i++) {
        const unsigned char faceShadow = (i == 0) ? 255 : (i == 1) ? 220 : 150;
        const Vector3 col = delete_mode ? brush_preview::deleteModeColor : brush_preview::createModeColor;

        DrawModel(models[i],
            center_of_model, 1.0f,
            Color{
                static_cast<unsigned char>(col.x / 255.0f * faceShadow),
                static_cast<unsigned char>(col.y / 255.0f * faceShadow),
                static_cast<unsigned char>(col.z / 255.0f * faceShadow),
                200
            });
    }
}

void brush_preview::draw_mesh_faces_on_render_tex(Renderer * renderer, RenderTexture2D &render_tex,
        const std::array<Model, 3> &models, const Vector3 center_of_model, const bool delete_mode) {
    if (render_tex.texture.width != GetScreenWidth() || render_tex.texture.height != GetScreenHeight()) {
        // IsWindowResized() does not work consistently?? for some reason
        UnloadRenderTexture(render_tex);
        render_tex = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }

    // For some reason without drawing at least 1 vertex before the model it doesn't render properly
    // (likely something to do with depth buffer??)
    // So this is an empty line that doesn't show up but this is necessary
    DrawLine3D(Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, WHITE);

    BeginTextureMode(render_tex);
    BeginMode3D(renderer->get_cam()->camera);
        ClearBackground(BLANK);
        brush_preview::draw_mesh_faces(models, center_of_model, delete_mode);
    EndMode3D();
    EndTextureMode();
}
