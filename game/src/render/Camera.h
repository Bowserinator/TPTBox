#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "rcamera.h"

#define CAMERA_MOVE_SPEED 4.0f
#define CAMERA_ROTATION_SPEED 0.06f
#define CAMERA_PAN_SPEED 0.5f

// Camera mouse movement sensitivity
#define CAMERA_MOUSE_MOVE_SENSITIVITY 0.003f // TODO: it should be independant of framerate
#define CAMERA_MOUSE_SCROLL_SENSITIVITY 1.5f

#define CAMERA_ORBITAL_SPEED 0.5f // Radians per second

#define CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER 8.0f
#define CAMERA_FIRST_PERSON_STEP_DIVIDER 30.0f
#define CAMERA_FIRST_PERSON_WAVING_DIVIDER 200.0f

// PLAYER (used by camera)
#define PLAYER_MOVEMENT_SENSITIVITY 20.0f

void UpdateSimCamera(Camera *camera) {
    Vector2 mousePositionDelta = GetMouseDelta();

    bool moveInWorldPlane = true;
    bool rotateAroundTarget = true;
    bool lockView = true;
    bool rotateUp = false;

    // Camera rotation
    if (IsKeyDown(KEY_DOWN))
        CameraPitch(camera, -CAMERA_ROTATION_SPEED, lockView, rotateAroundTarget, rotateUp);
    if (IsKeyDown(KEY_UP))
        CameraPitch(camera, CAMERA_ROTATION_SPEED, lockView, rotateAroundTarget, rotateUp);
    if (IsKeyDown(KEY_RIGHT))
        CameraYaw(camera, -CAMERA_ROTATION_SPEED, rotateAroundTarget);
    if (IsKeyDown(KEY_LEFT))
        CameraYaw(camera, CAMERA_ROTATION_SPEED, rotateAroundTarget);
    if (IsKeyDown(KEY_Q))
        CameraRoll(camera, -CAMERA_ROTATION_SPEED);
    if (IsKeyDown(KEY_E))
        CameraRoll(camera, CAMERA_ROTATION_SPEED);

    // Camera movement
    // Camera pan (for CAMERA_FREE)
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        const Vector2 mouseDelta = GetMouseDelta();
        if (mouseDelta.x > 0.0f)
            CameraMoveRight(camera, CAMERA_PAN_SPEED, moveInWorldPlane);
        if (mouseDelta.x < 0.0f)
            CameraMoveRight(camera, -CAMERA_PAN_SPEED, moveInWorldPlane);
        if (mouseDelta.y > 0.0f)
            CameraMoveUp(camera, -CAMERA_PAN_SPEED);
        if (mouseDelta.y < 0.0f)
            CameraMoveUp(camera, CAMERA_PAN_SPEED);
    }

    // Keyboard support
    if (IsKeyDown(KEY_W))
        CameraMoveForward(camera, CAMERA_MOVE_SPEED, moveInWorldPlane);
    if (IsKeyDown(KEY_A))
        CameraMoveRight(camera, -CAMERA_MOVE_SPEED, moveInWorldPlane);
    if (IsKeyDown(KEY_S))
        CameraMoveForward(camera, -CAMERA_MOVE_SPEED, moveInWorldPlane);
    if (IsKeyDown(KEY_D))
        CameraMoveRight(camera, CAMERA_MOVE_SPEED, moveInWorldPlane);
    if (IsKeyDown(KEY_SPACE))
        CameraMoveUp(camera, CAMERA_MOVE_SPEED);
    if (IsKeyDown(KEY_LEFT_SHIFT))
        CameraMoveUp(camera, -CAMERA_MOVE_SPEED);

    // Gamepad movement
    if (IsGamepadAvailable(0)) {
        // Gamepad controller support
        CameraYaw(camera, -(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * 2) * CAMERA_MOUSE_MOVE_SENSITIVITY, rotateAroundTarget);
        CameraPitch(camera, -(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * 2) * CAMERA_MOUSE_MOVE_SENSITIVITY, lockView, rotateAroundTarget, rotateUp);

        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) <= -0.25f)
            CameraMoveForward(camera, CAMERA_MOVE_SPEED, moveInWorldPlane);
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) <= -0.25f)
            CameraMoveRight(camera, -CAMERA_MOVE_SPEED, moveInWorldPlane);
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) >= 0.25f)
            CameraMoveForward(camera, -CAMERA_MOVE_SPEED, moveInWorldPlane);
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) >= 0.25f)
            CameraMoveRight(camera, CAMERA_MOVE_SPEED, moveInWorldPlane);
    }

    // Zoom in / out
    CameraMoveToTarget(camera, -12 * GetMouseWheelMove());
    if (IsKeyPressed(KEY_KP_SUBTRACT))
        CameraMoveToTarget(camera, 6.0f);
    if (IsKeyPressed(KEY_KP_ADD))
        CameraMoveToTarget(camera, -6.0f);
}


#endif