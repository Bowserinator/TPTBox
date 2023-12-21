#ifndef RENDER_CAMERA_H
#define RENDER_CAMERA_H

#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "rlgl.h"

#include "../types/plane.h"
#include "../../util/vector_op.h"

#include <vector>

/* A wrapper around Raylib's builtin Camera3D */
class RenderCamera {
public:
    Camera3D camera;
    Matrix viewProjMatrix;
    std::vector<Plane> frustum;

    RenderCamera(): camera{0}, _viewProjMatrixUpdated(false) {
        frustum = std::vector<Plane>(6);
    }

    /**
     * @brief Check if a sphere is outside the camera frustum
     * 
     * @param x Sphere center X
     * @param y Sphere center Y
     * @param z Sphere center Z
     * @param r Sphere radius
     * @return Whether sphere is outside view frustum
     */
    bool sphereOutsideFrustum(float x, float y, float z, float r);

    /**
     * @brief Update camera controls from user input, call this at the beginning
     * of each frame
     */
    void updateControls();


    // Use these methods to move / rotate camera as it properly
    // updates the frustum cache
    void pitch(float angle, bool lockView = true, bool rotateAroundTarget = true, bool rotateUp = false) {
        _viewProjMatrixUpdated = false;
        CameraPitch(&camera, angle, lockView, rotateAroundTarget, rotateUp);
    }
    void yaw(float angle, bool rotateAroundTarget = true) {
        _viewProjMatrixUpdated = false;
        CameraYaw(&camera, angle, rotateAroundTarget);
    }
    void roll(float angle) {
        _viewProjMatrixUpdated = false;
        CameraRoll(&camera, angle);
    }
    void moveForward(float distance, bool moveInWorldPlane = true) {
        _viewProjMatrixUpdated = false;
        CameraMoveForward(&camera, distance, moveInWorldPlane);
    }
    void moveRight(float distance, bool moveInWorldPlane = true) {
        _viewProjMatrixUpdated = false;
        CameraMoveRight(&camera, distance, moveInWorldPlane);
    }
    void moveUp(float distance) {
        _viewProjMatrixUpdated = false;
        CameraMoveUp(&camera, distance);
    }
    void moveToTarget(float amt) {
        _viewProjMatrixUpdated = _viewProjMatrixUpdated && amt == 0.0f;
        CameraMoveToTarget(&camera, amt);
    }
private:
    bool _viewProjMatrixUpdated;

    void generateFrustum();
    void updateViewProjMatrix();
};

#endif
