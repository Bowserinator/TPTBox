#ifndef INTERFACE_BRUSH_TOOLS_SIGNTOOL_H_
#define INTERFACE_BRUSH_TOOLS_SIGNTOOL_H_

#include "BrushShapeTool.h"
#include "../../../globals.h"
#include "../windows/SignWindow.h"
#include "../../../simulation/Sign.h"
#include "../../../util/vector_op.h"

class SignBrushTool : public BrushShapeTool {
public:
    SignBrushTool(): BrushShapeTool("Sign", 1) {}

    void operation(CLICK_BRUSH_OP_PARAMS) override {
        const auto &start = points[0];

        for (auto &sign : sim->signs.get_signs())
            if (is_sign_clicked(sign)) {
                sim_ui.addChild(new SignWindow(Vector2{(float)GetScreenWidth() / 2 - 250,
                    (float)GetScreenHeight() / 2 - 170 / 2},
                    Vector2{500, 170}, sim, Vector3T<int>(sign.x, sign.y, sign.z),
                    &sign));
                return;
            }

        if (sim->signs.sign_count() >= SimulationSigns::MAX_SIGNS) {
            sim_ui.error_popup("You've reached the maximum number of signs (" +
                std::to_string(SimulationSigns::MAX_SIGNS) + ")");
            return;
        }
        sim_ui.addChild(new SignWindow(Vector2{(float)GetScreenWidth() / 2 - 250,
            (float)GetScreenHeight() / 2 - 170 / 2},
            Vector2{500, 170}, sim, start));
    }

    void draw(CLICK_BRUSH_DRAW_PARAMS) {
        auto centerOfModel = (Vector3)pos;
        brush_preview::setup_shader(centerOfModel, renderer);
        brush_preview::draw_mesh_faces_on_render_tex(renderer, brush_preview::brush_tool_render_tex,
            preview_models, centerOfModel, false);
        util::draw_render_texture(brush_preview::brush_tool_render_tex);
    }

    void remesh(CLICK_BRUSH_REMESH_PARAMS) {
        if (IsModelReady(preview_models[0]))
            return;
        preview_models = brush_preview::generate_cuboid_faces(1, 1, 1);
    }

private:
    bool is_sign_clicked(const Sign &sign) {
        const Vector3 position = Vector3{ (float)sign.x, (float)sign.y, (float)sign.z };
        const auto camera = render_camera.camera;
        const Vector3 up = {0, 1, 0};
        const Vector2 size = Vector2{ SIGN_UPSCALE, SIGN_UPSCALE };
        const Rectangle source = {-1, -1, MAX_SIGN_WIDTH, -SIGN_HEIGHT};

        // NOTE: Billboard size will maintain source rectangle aspect ratio, size will represent billboard width
        Vector2 sizeRatio = { size.x * fabsf((float)source.width / source.height), size.y };
        Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);
        Vector3 right = { matView.m0, matView.m4, matView.m8 };

        Vector3 rightScaled = Vector3Scale(right, sizeRatio.x / 2);
        Vector3 upScaled = Vector3Scale(up, sizeRatio.y / 2);
        Vector3 p1 = Vector3Add(rightScaled, upScaled);
        Vector3 p2 = Vector3Subtract(rightScaled, upScaled);

        Vector3 topLeft = Vector3Scale(p2, -1) + position;
        Vector3 topRight = p1 + position;
        Vector3 bottomRight = p2 + position;
        Vector3 bottomLeft = Vector3Scale(p1, -1) + position;

        return GetRayCollisionQuad(
            GetMouseRay(GetMousePosition(), camera),
            topLeft, topRight, bottomLeft, bottomRight
        ).hit;
    }
};

#endif // INTERFACE_BRUSH_TOOLS_SIGNTOOL_H_
