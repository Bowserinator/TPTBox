#include "Brush.h"
#include "../../simulation/Simulation.h"
#include "../settings/data/SettingsData.h"
#include "../../simulation/ToolClasses.h"
#include "../../render/camera/camera.h"
#include "../../render/Renderer.h"
#include "../EventConsumer.h"
#include "../FrameTimeAvg.h"
#include "../../util/math.h"

void BrushRenderer::draw(Renderer * renderer) {
    if (x < 0 || y < 0 || z < 0) return;

    BeginMode3D(renderer->get_cam()->camera);

    if (offset != 0) {
        DrawCube(Vector3{ (float)x, (float)y, (float)z }, 1.0f, 1.0f, 1.0f, WHITE);

        // Draw the little dotted line to indicate offset
        constexpr float dotSize = 0.5f;
        constexpr int spacing = 2;
        const int half_size_x = size.x / 2;
        const int half_size_y = size.x / 2;
        const int half_size_z = size.x / 2;

        if (bx - half_size_x > x)
            for (float dx = x + 1; dx < bx - half_size_x; dx += spacing)
                DrawCube(Vector3{ dx, (float)y, (float)z }, dotSize, dotSize, dotSize, WHITE);
        else if (by - half_size_y > y)
            for (float dy = y + 1; dy < by - half_size_y; dy += spacing)
                DrawCube(Vector3{ (float)x, dy, (float)z }, dotSize, dotSize, dotSize, WHITE);
        else if (bz - half_size_z > z)
            for (float dz = z + 1; dz < bz - half_size_z; dz += spacing)
                DrawCube(Vector3{ (float)x, (float)y, dz }, dotSize, dotSize, dotSize, WHITE);
        else if (bx + half_size_x < x)
            for (float dx = x - 1; dx > bx + half_size_x; dx -= spacing)
                DrawCube(Vector3{ dx, (float)y, (float)z }, dotSize, dotSize, dotSize, WHITE);
        else if (by + half_size_y < y)
            for (float dy = y - 1; dy > by + half_size_y; dy -= spacing)
                DrawCube(Vector3{ (float)x, dy, (float)z }, dotSize, dotSize, dotSize, WHITE);
        else if (bz + half_size_z < z)
            for (float dz = z - 1; dz > bz + half_size_z; dz -= spacing)
                DrawCube(Vector3{ (float)x, (float)y, dz }, dotSize, dotSize, dotSize, WHITE);
    }

    if ((previousBrushIdx != currentBrushIdx || previousSize != size) &&
            (GetTime() - last_remesh_time) >= MIN_BRUSH_REMESH_DELAY_SECONDS) {
        last_remesh_time = GetTime();
        previousBrushIdx = currentBrushIdx;
        previousSize = size;
        current_brush_mesh.unload();
        current_brush_mesh = BrushFaceModels::GenBrushModel(BRUSHES[previousBrushIdx],
            Vector3T<int>(size.x, size.y, size.z));

        for (auto &f : change_callbacks)
            f();
    }

    current_brush_mesh.draw(
        Vector3T<int>(bx, by, bz),
        renderer,
        EventConsumer::ref()->isKeyDown(KEY_LEFT_SHIFT)
    );

    EndMode3D();
}

void BrushRenderer::update() {
    do_raycast(sim, camera);
    do_controls(sim);
}

void BrushRenderer::do_controls(Simulation * sim) {
    bool consumeMouse = false;
    const float deltaAvg = FrameTime::ref()->getDelta();
    const float scroll = EventConsumer::ref()->getMouseWheelMove();

    // TAB or SHIFT + TAB to change brush
    if (EventConsumer::ref()->isKeyPressed(KEY_TAB) && !EventConsumer::ref()->isKeyDown(KEY_LEFT_SHIFT)) {
        currentBrushIdx = (currentBrushIdx + 1) % BRUSHES.size();
        set_brush_type(currentBrushIdx);
    } else if (EventConsumer::ref()->isKeyPressed(KEY_TAB) && EventConsumer::ref()->isKeyDown(KEY_LEFT_SHIFT)) {
        if (currentBrushIdx == 0) currentBrushIdx = BRUSHES.size() - 1;
        else currentBrushIdx--;
        set_brush_type(currentBrushIdx);
    }

    // LCtrl + scroll to change brush size
    if (EventConsumer::ref()->isKeyDown(KEY_LEFT_CONTROL) && scroll) {
        size.x += std::round(scroll * deltaAvg * TARGET_FPS);
        size.y += std::round(scroll * deltaAvg * TARGET_FPS);
        size.z += std::round(scroll * deltaAvg * TARGET_FPS);
        size.x = util::clamp(size.x, 1, MAX_BRUSH_SIZE);
        size.y = util::clamp(size.y, 1, MAX_BRUSH_SIZE);
        size.z = util::clamp(size.z, 1, MAX_BRUSH_SIZE);
        consumeMouse = true;
    }

    // LShift + scroll to change brush offset
    if (EventConsumer::ref()->isKeyDown(KEY_LEFT_SHIFT) && scroll) {
        offset += std::round(scroll * deltaAvg * TARGET_FPS);
        consumeMouse = true;
        update_offset();
    }

    // LClick to place parts
    if (EventConsumer::ref()->isMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        consumeMouse = true;
        const bool delete_mode = EventConsumer::ref()->isKeyDown(KEY_LEFT_SHIFT);
        const Vector3 sizeVec = (Vector3)size;
        const Vector3 rotVec{0.0f, 0.0f, 0.0f};

        const Vector3 viewBegin = settings::data::ref()->graphics->viewSliceBegin;
        const Vector3 viewEnd = settings::data::ref()->graphics->viewSliceEnd;

        for (int x = bx - (int)size.x / 2; x <= bx + (int)size.x / 2; x++)
        for (int y = by - (int)size.y / 2; y <= by + (int)size.y / 2; y++)
        for (int z = bz - (int)size.z / 2; z <= bz + (int)size.z / 2; z++)
            if (
                BOUNDS_CHECK(x, y, z) &&
                BRUSHES[currentBrushIdx].map(
                    Vector3{(float)(x - bx), (float)(y - by), (float)(z - bz)},
                    sizeVec, rotVec) &&
                x >= viewBegin.x && y >= viewBegin.y && z >= viewBegin.z &&
                x <= viewEnd.x && y <= viewEnd.y && z <= viewEnd.z
            ) {
                if (delete_mode)
                    sim->kill_part(ID(sim->pmap[z][y][x]));
                else if (!tool_mode)
                    sim->create_part(x, y, z, selected_element, PartCreateMode::BRUSH);
                else {
                    auto &tool = GetTools()[selected_tool];
                    if (tool.Perform) {
                        int i = ID(sim->pmap[z][y][x]);
                        if (!i) i = ID(sim->photons[z][y][x]);
                        tool.Perform(*sim, i, x, y, z,
                            sim->parts, sim->pmap, bx, by, bz,
                            (size.x + size.y + size.z) / 3.0f, misc_data);
                    }
                }
            }
    }

    if (consumeMouse)
        EventConsumer::ref()->consumeMouse();
}

void BrushRenderer::do_raycast(Simulation * sim, RenderCamera * camera) {
    const auto mousePos = GetMousePosition();
    if (mousePos == prevMousePos)
        return;

    Ray ray = GetMouseRay(mousePos, camera->camera);
    int cx = std::round(camera->camera.position.x);
    int cy = std::round(camera->camera.position.y);
    int cz = std::round(camera->camera.position.z);

    // Camera is outside the cube, project ray into first position
    // that's actually in the cube
    if (REVERSE_BOUNDS_CHECK(cx, cy, cz)) {
        const BoundingBox simulationBounds {
            .min = Vector3{ 1.0f, 1.0f, 1.0f },
            .max = Vector3{ XRES - 2.0f, YRES - 2.0f, ZRES - 2.0f }
        };
        const auto collide = GetRayCollisionBox(ray, simulationBounds);

        if (!collide.hit) {
            x = y = z = -1; // Invalidate cursor location
            return;
        }
        cx = std::round(collide.point.x);
        cy = std::round(collide.point.y);
        cz = std::round(collide.point.z);
    }

    auto pmapOccupied = [sim](const Vector3T<signed_coord_t> &loc) -> PartSwapBehavior {
        if (REVERSE_BOUNDS_CHECK(loc.x, loc.y, loc.z))
            return PartSwapBehavior::NOOP;
        if (sim->pmap[loc.z][loc.y][loc.x] || sim->photons[loc.z][loc.y][loc.x])
            return PartSwapBehavior::NOOP;
        return PartSwapBehavior::SWAP;
    };

    RaycastOutput out;
    ray.direction *= (XRES + YRES + ZRES);
    sim->raycast<true, true>(RaycastInput {
        .x = (coord_t)cx, .y = (coord_t)cy, .z = (coord_t)cz,
        .vx = ray.direction.x, .vy = ray.direction.y, .vz = ray.direction.z
    }, out, pmapOccupied);

    prevMousePos = mousePos;
    prevCameraPos = camera->camera.position;
    prevSimFrameCount = sim->frame_count;
    this->x = this->bx = out.x;
    this->y = this->by = out.y;
    this->z = this->bz = out.z;

    raycast_out = out;
    update_offset();
}

void BrushRenderer::update_offset() {
    if (offset != 0) {
        if ((raycast_out.faces & RayCast::FACE_X).any())
            this->bx = this->x + offset * util::sign(camera->camera.position.x - this->x);
        else if ((raycast_out.faces & RayCast::FACE_Y).any())
            this->by = this->y + offset * util::sign(camera->camera.position.y - this->y);
        else if ((raycast_out.faces & RayCast::FACE_Z).any())
            this->bz = this->z + offset * util::sign(camera->camera.position.z - this->z);
    }
}

void BrushRenderer::set_brush_type(std::size_t currentBrushIdx) {
    this->currentBrushIdx = currentBrushIdx;
    setCurrentTooltip("Brush: " + BRUSHES[currentBrushIdx].name);
}
