#include "NavCube.h"
#include "../../render/camera/camera.h"
#include "../../simulation/SimulationDef.h"
#include "../../util/util.h"

#include <cmath>
#include <iostream>
#include "rlgl.h"

constexpr int CUBE_FACE_IMG_SIZE = 100;
constexpr int CUBE_FACE_FONT_SIZE = 20;

enum CUBE_FACES {
    FRONT = 0,
    BACK = 1,
    LEFT = 2,
    RIGHT = 3,
    TOP = 4,
    BOTTOM = 5
};

// Make sure to call this after OpenGL context has been initialized!
RenderTexture2D generateCubeFaceTexture(const char * faceName) {
    auto tex = LoadRenderTexture(CUBE_FACE_IMG_SIZE, CUBE_FACE_IMG_SIZE);
    int twidth = MeasureText(faceName, CUBE_FACE_FONT_SIZE);

    BeginTextureMode(tex);
        ClearBackground(Color { 0, 0, 0, 255 });
        DrawText(faceName,
            CUBE_FACE_IMG_SIZE / 2 - twidth / 2, 
            CUBE_FACE_IMG_SIZE / 2 - CUBE_FACE_FONT_SIZE / 2,
            CUBE_FACE_FONT_SIZE, WHITE);
    EndTextureMode();

    return tex;
}

NavCube::NavCube(RenderCamera * cam): cam(cam) {}

void NavCube::init() {
    target = LoadRenderTexture(NAV_CUBE_WINDOW_SIZE, NAV_CUBE_WINDOW_SIZE);
    local_cam = {0};
    local_cam.position = Vector3{0.0f, 0.0f, 1.7f};
    local_cam.projection = CAMERA_PERSPECTIVE;
    local_cam.target = Vector3{0.0f, 0.0f, 0.0f};
    local_cam.up = Vector3{0.0f, 1.0f, 0.0f};
    local_cam.fovy = 70.0f;

    cube_faces[FRONT]  = generateCubeFaceTexture("Front");
    cube_faces[BACK]   = generateCubeFaceTexture("Back");
    cube_faces[LEFT]   = generateCubeFaceTexture("Left");
    cube_faces[RIGHT]  = generateCubeFaceTexture("Right");
    cube_faces[TOP]    = generateCubeFaceTexture("Top");
    cube_faces[BOTTOM] = generateCubeFaceTexture("Bottom");
}

void NavCube::update() {
    BeginTextureMode(target);
    ClearBackground(Color{0, 0, 0, 127}); // Semi-transparent background

    Matrix transform_mat = MatrixLookAt(cam->camera.position, cam->camera.target, cam->camera.up);
    util::reduce_to_rotation(transform_mat);
    
    BeginMode3D(local_cam);
        rlPushMatrix();
        rlMultMatrixf(MatrixToFloatV(transform_mat).v);

        DrawCubeCustom();
        DrawCubeWires(Vector3{0.0f, 0.0f, 0.0f}, 1.0f, 1.0f, 1.0f, WHITE);

        { // Draw le coordinate axii, X = red, Y = green, Z = blue
            constexpr float lineThickness = 0.01f;
            constexpr float lineLength = 1.05f;
            constexpr float coneHeight = 0.05f;
            constexpr Vector3 origin{-0.5f, -0.5f, -0.5f};
            constexpr Color axisColors[3] = { RED, GREEN, BLUE };

            for (int axis = 0; axis < 3; axis++) {
                // The actual line
                const Vector3 end{
                    origin.x + lineLength * (axis == 0),
                    origin.y + lineLength * (axis == 1),
                    origin.z + lineLength * (axis == 2)
                };
                DrawCylinderEx(origin, end, lineThickness, lineThickness, 7, axisColors[axis]);
                DrawCylinderEx(end, Vector3{
                    end.x + coneHeight * (axis == 0),
                    end.y + coneHeight * (axis == 1),
                    end.z + coneHeight * (axis == 2)
                }, lineThickness * 3, lineThickness, 7, axisColors[axis]);

            }
        }
        rlPopMatrix();
    EndMode3D();

    EndTextureMode(); 

    DrawTextureRec(target.texture,
        Rectangle{ 0, 0, (float)target.texture.width, (float)-target.texture.height },
        Vector2{ 20, 70 }, WHITE);
}


void NavCube::DrawCubeCustom() {
    constexpr float width = 1.0f;
    constexpr float height = 1.0f;
    constexpr float length = 1.0f;

    constexpr float x = 0.0f;
    constexpr float y = 0.0f;
    constexpr float z = 0.0f;

    rlBegin(RL_QUADS);
    rlColor4ub(255, 255, 255, 255);

    // Front Face
    rlSetTexture(cube_faces[FRONT].texture.id);
    rlNormal3f(0.0f, 0.0f, 1.0f);

    rlTexCoord2f(0.0f, 0.0f); // BL
    rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 0.0f); // BR
    rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 1.0f); // TR
    rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(0.0f, 1.0f); // TL
    rlVertex3f(x - width / 2, y + height / 2, z + length / 2);

    // Back Face
    rlSetTexture(cube_faces[BACK].texture.id);
    rlNormal3f(0.0f, 0.0f, -1.0f);

    rlTexCoord2f(1.0f, 0.0f);
    rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(1.0f, 1.0f);
    rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 1.0f);
    rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(x + width / 2, y - height / 2, z - length / 2);

    // Top Face
    rlSetTexture(cube_faces[TOP].texture.id);
    rlNormal3f(0.0f, 1.0f, 0.0f);

    rlTexCoord2f(0.0f, 1.0f);
    rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 0.0f);
    rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 1.0f);
    rlVertex3f(x + width / 2, y + height / 2, z - length / 2);

    // Bottom Face
    rlSetTexture(cube_faces[BOTTOM].texture.id);
    rlNormal3f(0.0f, -1.0f, 0.0f);
    rlTexCoord2f(1.0f, 1.0f);
    rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 1.0f);
    rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 0.0f);
    rlVertex3f(x - width / 2, y - height / 2, z + length / 2);

    // Right face
    rlSetTexture(cube_faces[RIGHT].texture.id);
    rlNormal3f(1.0f, 0.0f, 0.0f);
    rlTexCoord2f(1.0f, 0.0f);
    rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(1.0f, 1.0f);
    rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
    rlTexCoord2f(0.0f, 1.0f);
    rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(x + width / 2, y - height / 2, z + length / 2);

    // Left Face
    rlSetTexture(cube_faces[LEFT].texture.id);
    rlNormal3f(-1.0f, 0.0f, 0.0f);
    rlTexCoord2f(0.0f, 0.0f);
    rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
    rlTexCoord2f(1.0f, 0.0f);
    rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
    rlTexCoord2f(1.0f, 1.0f);
    rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
    rlTexCoord2f(0.0f, 1.0f);
    rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
    rlEnd();

    rlSetTexture(0);
}