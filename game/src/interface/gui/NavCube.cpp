#include "NavCube.h"
#include "../../render/camera/camera.h"
#include "../../simulation/SimulationDef.h"
#include "../../util/util.h"
#include "../FontCache.h"
#include "../EventConsumer.h"

#include <cmath>
#include <iostream>
#include "rlgl.h"

constexpr int CUBE_FACE_IMG_SIZE = 100;
constexpr int CUBE_FACE_FONT_SIZE = 13;
constexpr Vector2 NAVCUBE_POS{ 15, 70 };

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
    constexpr float SPACING = -0.5f;
    auto tex = LoadRenderTexture(CUBE_FACE_IMG_SIZE, CUBE_FACE_IMG_SIZE);
    Vector2 tsize = MeasureTextEx(FontCache::ref()->main_font, faceName, CUBE_FACE_FONT_SIZE, SPACING);

    BeginTextureMode(tex);
        ClearBackground(Color { 0, 0, 0, 255 });
        DrawTextEx(FontCache::ref()->main_font, faceName,
            Vector2{
                CUBE_FACE_IMG_SIZE / 2 - tsize.x / 2, 
                CUBE_FACE_IMG_SIZE / 2 - tsize.y / 2
            },
            CUBE_FACE_FONT_SIZE, SPACING, WHITE);

        // Face outline
        DrawLine(1, 1, CUBE_FACE_IMG_SIZE - 1, 0, WHITE);
        DrawLine(1, 1, 1, CUBE_FACE_IMG_SIZE - 1, WHITE);
        DrawLine(CUBE_FACE_IMG_SIZE - 1, CUBE_FACE_IMG_SIZE - 1, CUBE_FACE_IMG_SIZE - 1, 1, WHITE);
        DrawLine(CUBE_FACE_IMG_SIZE - 1, CUBE_FACE_IMG_SIZE - 1, 1, CUBE_FACE_IMG_SIZE - 1, WHITE);
    EndTextureMode();

    return tex;
}

NavCube::NavCube(RenderCamera * cam): cam(cam) {}

void NavCube::init() {
    cam_hash = 99999;

    target = LoadRenderTexture(NAV_CUBE_WINDOW_SIZE, NAV_CUBE_WINDOW_SIZE);
    local_cam = {0};
    local_cam.position = Vector3{0.0f, 0.0f, 1.7f};
    local_cam.projection = cam->camera.projection;
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

/**
 * @brief This is copy paste of Raylib's GetMouseRay, but the aspect ratio
 *        and screen dimensions are hard-coded in that function, so we have to
 *        copy paste it here and replace the aspect ratio with 1.0f
 * 
 * @param mouse Mouse coords clicked (global, will be converted to local)
 * @param camera Local camera
 * @return Ray 
 */
Ray NavCubeGetMouseRay(Vector2 mouse, const Camera &camera) {
    Ray ray = { 0 };

    // Consider top left to be mini window bound
    mouse.x -= NAVCUBE_POS.x;
    mouse.y -= NAVCUBE_POS.y;

    float x = (2.0f * mouse.x) / (float)NAV_CUBE_WINDOW_SIZE - 1.0f;
    float y = 1.0f - (2.0f * mouse.y) / (float)NAV_CUBE_WINDOW_SIZE;
    float z = 1.0f;

    Vector3 deviceCoords = { x, y, z };
    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);
    Matrix matProj = MatrixIdentity();

    if (camera.projection == CAMERA_PERSPECTIVE)
        matProj = MatrixPerspective(camera.fovy * DEG2RAD, 1.0f, RL_CULL_DISTANCE_NEAR, RL_CULL_DISTANCE_FAR);
    else if (camera.projection == CAMERA_ORTHOGRAPHIC) {
        double aspect = 1.0f;
        double top = camera.fovy / 2.0;
        double right = top*aspect;
        matProj = MatrixOrtho(-right, right, -top, top, 0.01, 1000.0);
    }

    Vector3 nearPoint = Vector3Unproject(Vector3{ deviceCoords.x, deviceCoords.y, 0.0f }, matProj, matView);
    Vector3 farPoint = Vector3Unproject(Vector3{ deviceCoords.x, deviceCoords.y, 1.0f }, matProj, matView);
    Vector3 cameraPlanePointerPos = Vector3Unproject(Vector3{ deviceCoords.x, deviceCoords.y, -1.0f }, matProj, matView);

    Vector3 direction = Vector3Normalize(Vector3Subtract(farPoint, nearPoint));
    if (camera.projection == CAMERA_PERSPECTIVE) ray.position = camera.position;
    else if (camera.projection == CAMERA_ORTHOGRAPHIC) ray.position = cameraPlanePointerPos;
    ray.direction = direction;
    return ray;
}


void NavCube::update() {
    // Begin drawing
    BeginTextureMode(target);
    ClearBackground(Color{0, 0, 0, 127}); // Semi-transparent background

    if (cam->hash() != cam_hash) {
        transform_mat = MatrixLookAt(cam->camera.position, cam->camera.target, cam->camera.up);
        util::reduce_to_rotation(transform_mat);
        transform_mat_cache = MatrixToFloatV(transform_mat);
        cam_hash = cam->hash();
    }
    
    BeginMode3D(local_cam);
        rlPushMatrix();
        rlMultMatrixf(transform_mat_cache.v);

        DrawCubeCustom();

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


    // Mouse click in the nav cube window
    const auto mousePos = GetMousePosition();
    bool mouseInBounds = (mousePos.x >= NAVCUBE_POS.x && mousePos.x <= NAVCUBE_POS.x + NAV_CUBE_WINDOW_SIZE) &&
                         (mousePos.y >= NAVCUBE_POS.y && mousePos.y <= NAVCUBE_POS.y + NAV_CUBE_WINDOW_SIZE);
    bool clicked = EventConsumer::ref()->isMouseButtonPressed(MOUSE_BUTTON_LEFT);

    if (clicked && mouseInBounds) {
        Ray ray = NavCubeGetMouseRay(GetMousePosition(), local_cam);

        // Undo rotation matrix (R^T is the inverse for rot matrix)
        // We imagine the cube is fixed and we rotate the ray cast from the camera position
        // the opposite way the cube was rotated
        const auto transform_mat_T = MatrixTranspose(transform_mat);
        ray.direction = Vector3Transform(ray.direction, transform_mat_T);
        ray.position = Vector3Transform(ray.position, transform_mat_T);

        auto collide = GetRayCollisionBox(ray, BoundingBox {
            .min = Vector3{ -0.52f, -0.52f, -0.52f },
            .max = Vector3{ 0.52f, 0.52f, 0.52f }
        });
        if (collide.hit) {
            constexpr float EDGE_MARGIN = 0.08f; // If click is this close to edge, consider it clicking the edge
            bool faces_clicked[6] = { false, false, false, false, false, false };

            if (collide.point.z > 0.5f - EDGE_MARGIN)
                faces_clicked[FRONT] = true;
            else if (collide.point.z < -0.5f + EDGE_MARGIN)
                faces_clicked[BACK] = true;
            if (collide.point.x > 0.5f - EDGE_MARGIN)
                faces_clicked[RIGHT] = true;
            else if (collide.point.x < -0.5f + EDGE_MARGIN)
                faces_clicked[LEFT] = true;
            if (collide.point.y > 0.5f - EDGE_MARGIN)
                faces_clicked[TOP] = true;
            else if (collide.point.y < -0.5f + EDGE_MARGIN)
                faces_clicked[BOTTOM] = true;

            // Set lerp target based on faces
            Vector3 target_pos{ XRES / 2, YRES / 2, ZRES / 2 };
            Vector3 target{ XRES / 2, YRES / 2, ZRES / 2 };

            if (faces_clicked[TOP]) {
                target_pos.y = 2.5f * (float)YRES;
                target_pos.z = 0.5f * ZRES + 0.1f; // Can't be perfectly on top of target
            }
            else if (faces_clicked[BOTTOM]) {
                target_pos.y = -1.5f * (float)YRES;
                target_pos.z = 0.5f * ZRES - 0.1f; // Can't be perfectly on top of target
            }
            if (faces_clicked[FRONT])
                target_pos.z = 2.5f * (float)ZRES;
            else if (faces_clicked[BACK])
                target_pos.z = -1.5f * (float)ZRES;
            if (faces_clicked[RIGHT])
                target_pos.x = 2.5f * (float)XRES;
            else if (faces_clicked[LEFT])
                target_pos.x = -1.5f * (float)XRES;

            cam->setLerpTarget(target_pos, target, Vector3{0.0f, 1.0f, 0.0f});  
        }
    }
    if (mouseInBounds)
        EventConsumer::ref()->consumeMouse();

    DrawTextureRec(target.texture,
        Rectangle{ 0, 0, (float)target.texture.width, (float)-target.texture.height },
        NAVCUBE_POS, WHITE);
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