#ifndef NAV_CUBE_H
#define NAV_CUBE_H

#include "raylib.h"
#include "raymath.h"

constexpr int NAV_CUBE_WINDOW_SIZE = 160;

class RenderCamera;

class NavCube {
public:
    NavCube(RenderCamera * cam);
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

#endif