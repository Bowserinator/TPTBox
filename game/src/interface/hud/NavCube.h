#ifndef INTERFACE_HUD_NAVCUBE_H_
#define INTERFACE_HUD_NAVCUBE_H_

#include "raylib.h"
#include "raymath.h"

constexpr int NAV_CUBE_WINDOW_SIZE = 160;

class RenderCamera;

class NavCube {
public:
    explicit NavCube(RenderCamera * cam): cam(cam) {}
    ~NavCube() {
        UnloadRenderTexture(target);
        for (int i = 0; i < 6; i++)
            UnloadRenderTexture(cube_faces[i]);
    }

    void init(); // called after OpenGL instance is initialized
    void update();
    void draw();
private:
    RenderCamera * cam;
    RenderTexture2D target;
    Camera3D local_cam;
    RenderTexture2D cube_faces[6];

    float16 transform_mat_cache;
    Matrix transform_mat;
    int cam_hash;

    void DrawCubeCustom();
};

#endif // INTERFACE_HUD_NAVCUBE_H_
