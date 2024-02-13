#include "Brush.h"
#include "../../simulation/Simulation.h"
#include "../../render/camera/camera.h"
#include "../EventConsumer.h"
#include "../FrameTimeAvg.h"
#include "../../util/math.h"

void BrushRenderer::draw() {
    if (x < 0 || y < 0 || z < 0) return;
    if (offset != 0) {
        DrawCube(Vector3{ (float)x, (float)y, (float)z }, 1.0f, 1.0f, 1.0f, WHITE);
        
        // Draw the little dotted line to indicate offset
        constexpr float dotSize = 0.5f;
        constexpr int spacing = 2;
        const int half_size = size / 2;

        if (bx - half_size > x)
            for (float dx = x + 1; dx < bx - half_size; dx += spacing)
                DrawCube(Vector3{ dx, (float)y, (float)z }, dotSize, dotSize, dotSize, WHITE);
        else if (by - half_size > y)
            for (float dy = y + 1; dy < by - half_size; dy += spacing)
                DrawCube(Vector3{ (float)x, dy, (float)z }, dotSize, dotSize, dotSize, WHITE);
        else if (bz - half_size > z)
            for (float dz = z + 1; dz < bz - half_size; dz += spacing)
                DrawCube(Vector3{ (float)x, (float)y, dz }, dotSize, dotSize, dotSize, WHITE);
        else if (bx + half_size < x)
            for (float dx = x - 1; dx > bx + half_size; dx -= spacing)
                DrawCube(Vector3{ dx, (float)y, (float)z }, dotSize, dotSize, dotSize, WHITE);
        else if (by + half_size < y)
            for (float dy = y - 1; dy > by + half_size; dy -= spacing)
                DrawCube(Vector3{ (float)x, dy, (float)z }, dotSize, dotSize, dotSize, WHITE);
        else if (bz + half_size < z)
            for (float dz = z - 1; dz > bz + half_size; dz -= spacing)
                DrawCube(Vector3{ (float)x, (float)y, dz }, dotSize, dotSize, dotSize, WHITE);
    }
    DrawCubeWires(Vector3{ (float)bx, (float)by, (float)bz }, size, size, size, WHITE);
}

void BrushRenderer::update() {
    do_raycast(sim, camera);
    do_controls(sim);
}

void BrushRenderer::do_controls(Simulation * sim) {
    bool consumeMouse = false;
    const float deltaAvg = FrameTime::ref()->getDelta();
    const float scroll = EventConsumer::ref()->getMouseWheelMove();

    // LCtrl + scroll to change brush size
    if (EventConsumer::ref()->isKeyDown(KEY_LEFT_CONTROL) && scroll) {
        size += std::round(scroll * deltaAvg * TARGET_FPS);
        size = util::clamp(size, 1, (XRES + YRES + ZRES) * 2);
        consumeMouse = true;
    }

    // LShift + scroll to change brush offset
    if (EventConsumer::ref()->isKeyDown(KEY_LEFT_SHIFT) && scroll) {
        offset += std::round(scroll * deltaAvg * TARGET_FPS);
        consumeMouse = true;
        prevMousePos = Vector2{0.0f, 0.0f}; // Clear mouse pos cache since offset changes pos
    }

    // LClick to place parts
    if (EventConsumer::ref()->isMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        consumeMouse = true;
        const int S = 5;
        const bool delete_mode = EventConsumer::ref()->isKeyDown(KEY_LEFT_SHIFT);
        const int half_size = size / 2;

        for (int x = bx - half_size; x <= bx + half_size; x++)
        for (int y = by - half_size; y <= by + half_size; y++)
        for (int z = bz - half_size; z <= bz + half_size; z++)
            if (BOUNDS_CHECK(x, y, z)) {
                if (delete_mode)
                    sim->kill_part(ID(sim->pmap[z][y][x]));
                else
                    sim->create_part(x, y, z, selected_element);
            }
    }

    if (consumeMouse)
        EventConsumer::ref()->consumeMouse();
}

void BrushRenderer::do_raycast(Simulation * sim, RenderCamera * camera) {
    const auto mousePos = GetMousePosition();
    if (mousePos == prevMousePos && camera->camera.position == prevCameraPos && sim->frame_count == prevSimFrameCount)
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
    bool hit = sim->raycast<true, true>(RaycastInput {
        .x = (coord_t)cx, .y = (coord_t)cy, .z = (coord_t)cz,
        .vx = ray.direction.x, .vy = ray.direction.y, .vz = ray.direction.z
    }, out, pmapOccupied);

    prevMousePos = mousePos;
    prevCameraPos = camera->camera.position;
    prevSimFrameCount = sim->frame_count;
    this->x = this->bx = out.x;
    this->y = this->by = out.y;
    this->z = this->bz = out.z;

    if (offset != 0) {
        if ((out.faces & RayCast::FACE_X).any())
            this->bx = this->x + offset * util::sign(camera->camera.position.x - this->x);
        else if ((out.faces & RayCast::FACE_Y).any())
            this->by = this->y + offset * util::sign(camera->camera.position.y - this->y);
        else if ((out.faces & RayCast::FACE_Z).any())
            this->bz = this->z + offset * util::sign(camera->camera.position.z - this->z);
    }
}
