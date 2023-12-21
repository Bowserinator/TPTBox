#ifndef RENDER_CONSTANTS_H
#define RENDER_CONSTANTS_H

// Math
constexpr float DIS_UNIT_CUBE_CENTER_TO_CORNER = 0.8660254038f;

// Camera controls
constexpr float CAMERA_MOVE_SPEED = 4.0f;
constexpr float CAMERA_ROTATION_SPEED = 0.06f;
constexpr float CAMERA_PAN_SPEED = 0.5f;

constexpr float CAMERA_MOUSE_MOVE_SENSITIVITY = 0.003f; // TODO: it should be independent of framerate
constexpr float CAMERA_MOUSE_SCROLL_SENSITIVITY = 1.5f;

#endif