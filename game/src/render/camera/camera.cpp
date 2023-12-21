#include "camera.h"
#include "../constants.h"

bool RenderCamera::sphereOutsideFrustum(float x, float y, float z, float r) {
    // Update cache if invalidated
    if (!_viewProjMatrixUpdated) {
        updateViewProjMatrix();
        generateFrustum();
    }

    // If distance to plane is "below" the plane and more than
    // the radius of the sphere, the sphere is 100% below the plane
    // (outside the frustum, which is made of 6 planes)
    for (const auto &plane: frustum) {
        if (plane.distanceToPlane(x, y, z) < -r)
            return true;
    }
    return false;
}

void RenderCamera::updateControls() {
    Vector2 mousePositionDelta = GetMouseDelta();

    bool moveInWorldPlane = true;
    bool rotateAroundTarget = true;
    bool lockView = true;
    bool rotateUp = false;

    // Camera rotation
    if (IsKeyDown(KEY_DOWN))
        pitch(-CAMERA_ROTATION_SPEED, lockView, rotateAroundTarget, rotateUp);
    if (IsKeyDown(KEY_UP))
        pitch(CAMERA_ROTATION_SPEED, lockView, rotateAroundTarget, rotateUp);
    if (IsKeyDown(KEY_RIGHT))
        yaw(-CAMERA_ROTATION_SPEED, rotateAroundTarget);
    if (IsKeyDown(KEY_LEFT))
        yaw(CAMERA_ROTATION_SPEED, rotateAroundTarget);
    if (IsKeyDown(KEY_Q))
        roll(-CAMERA_ROTATION_SPEED);
    if (IsKeyDown(KEY_E))
        roll(CAMERA_ROTATION_SPEED);

    // Camera movement
    // Camera pan (for CAMERA_FREE)
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        const Vector2 mouseDelta = GetMouseDelta();
        if (mouseDelta.x > 0.0f)
            moveRight(CAMERA_PAN_SPEED, moveInWorldPlane);
        if (mouseDelta.x < 0.0f)
            moveRight(-CAMERA_PAN_SPEED, moveInWorldPlane);
        if (mouseDelta.y > 0.0f)
            moveUp(-CAMERA_PAN_SPEED);
        if (mouseDelta.y < 0.0f)
            moveUp(CAMERA_PAN_SPEED);
    }

    // Keyboard support
    if (IsKeyDown(KEY_W))
        moveForward(CAMERA_MOVE_SPEED, moveInWorldPlane);
    if (IsKeyDown(KEY_A))
        moveRight(-CAMERA_MOVE_SPEED, moveInWorldPlane);
    if (IsKeyDown(KEY_S))
        moveForward(-CAMERA_MOVE_SPEED, moveInWorldPlane);
    if (IsKeyDown(KEY_D))
        moveRight(CAMERA_MOVE_SPEED, moveInWorldPlane);
    if (IsKeyDown(KEY_SPACE))
        moveUp(CAMERA_MOVE_SPEED);
    if (IsKeyDown(KEY_LEFT_SHIFT))
        moveUp(-CAMERA_MOVE_SPEED);

    // Gamepad movement
    if (IsGamepadAvailable(0)) {
        // Gamepad controller support
        yaw(-(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * 2) * CAMERA_MOUSE_MOVE_SENSITIVITY, rotateAroundTarget);
        pitch(-(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * 2) * CAMERA_MOUSE_MOVE_SENSITIVITY, lockView, rotateAroundTarget, rotateUp);

        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) <= -0.25f)
            moveForward(CAMERA_MOVE_SPEED, moveInWorldPlane);
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) <= -0.25f)
            moveRight(-CAMERA_MOVE_SPEED, moveInWorldPlane);
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) >= 0.25f)
            moveForward(-CAMERA_MOVE_SPEED, moveInWorldPlane);
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) >= 0.25f)
            moveRight(CAMERA_MOVE_SPEED, moveInWorldPlane);
    }

    // Zoom in / out
    moveToTarget(-12 * GetMouseWheelMove());
    if (IsKeyPressed(KEY_KP_SUBTRACT))
        moveToTarget(6.0f);
    if (IsKeyPressed(KEY_KP_ADD))
        moveToTarget(-6.0f);
}


void RenderCamera::generateFrustum() {
    const auto &vp = viewProjMatrix;
    const Vector4 row1{ vp.m0, vp.m4, vp.m8, vp.m12 };
    const Vector4 row2{ vp.m1, vp.m5, vp.m9, vp.m13 };
    const Vector4 row3{ vp.m2, vp.m6, vp.m10, vp.m14 };
    const Vector4 row4{ vp.m3, vp.m7, vp.m11, vp.m15 };

    frustum[0] = Plane(row4 + row1);
    frustum[1] = Plane(row4 - row1);
    frustum[2] = Plane(row4 + row2);
    frustum[3] = Plane(row4 - row2);
    frustum[4] = Plane(row4 + row3);
    frustum[5] = Plane(row4 - row3);
}

void RenderCamera::updateViewProjMatrix() {
    const float aspectRatio = ((double)GetScreenWidth() / (double)GetScreenHeight());
    const Matrix matProj = MatrixPerspective(camera.fovy * DEG2RAD, aspectRatio, RL_CULL_DISTANCE_NEAR, RL_CULL_DISTANCE_FAR);
    const Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);
    viewProjMatrix = MatrixMultiply(matView, matProj);
    _viewProjMatrixUpdated = true;
}
