#ifndef RENDER_CONSTANTS_H
#define RENDER_CONSTANTS_H

#define EMBED_SHADERS

enum CUBE_FACES {
    FRONT = 0,
    BACK = 1,
    LEFT = 2,
    RIGHT = 3,
    TOP = 4,
    BOTTOM = 5
};

// Math
constexpr float DIS_UNIT_CUBE_CENTER_TO_CORNER = 0.8660254038f;

// Camera controls
constexpr float CAMERA_MOVE_SPEED = 3.0f;
constexpr float CAMERA_MOVE_SPEED_PAN = 4.0f;
constexpr float CAMERA_ROTATION_SPEED = 0.02f;
constexpr float CAMERA_FIRST_PERSON_ROTATION_SPEED = 0.003f;
constexpr float CAMERA_PAN_SPEED = 7.5f;
constexpr float CAMERA_ZOOM_SPEED = 6.0f;

constexpr float CAMERA_MOUSE_ROTATION_SPEED = 0.002f;
constexpr float CAMERA_MOUSE_ZOOM_SPEED = 12.0f;

#endif