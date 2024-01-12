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

    Vector3 minBound;
    Vector3 maxBound;

    RenderCamera(): camera{0}, _viewProjMatrixUpdated(false),
            _isLerping(false), _lerpSteps(0), _hash(0),
            minBound{INT_MIN, INT_MIN, INT_MIN},
            maxBound(INT_MAX, INT_MAX, INT_MAX)
    {
        frustum = std::vector<Plane>(6);
    }

    void setLerpTarget(const Vector3 &pos, const Vector3 &target, const Vector3 &up) {
        _isLerping = true;
        _lerpPos = pos;
        _lerpTarget = target;
        _lerpUp = up;
        _lerpSteps = 0;
    }

    void setBounds(const Vector3 &minBound, const Vector3 &maxBound) {
        this->minBound = minBound;
        this->maxBound = maxBound;
    }

    /** Can be used to check if camera state has been modified */
    int hash() const {
        return _hash;
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
     * @brief Update camera controls from user input + lerp, call this at the beginning
     * of each frame
     */
    void update();

    /**
     * @brief Get approximation of distance outside bounding box
     * @param pos Position to evaluate
     * @return Manhattan distance to nearest bounding planes if outside
     */
    float boundError(const Vector3& pos) const {
        float err = 0;
        if (pos.x < minBound.x) err += minBound.x - pos.x;
        else if (pos.x > maxBound.x) err += pos.x - maxBound.x;
        if (pos.y < minBound.y) err += minBound.y - pos.y;
        else if (pos.y > maxBound.y) err += pos.y - maxBound.y;
        if (pos.z < minBound.z) err += minBound.z - pos.z;
        else if (pos.z > maxBound.z) err += pos.z - maxBound.z;
        return err;
    }


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

    // These camera functions constrain movement and update
    // the projection matrix cache
    void moveForward(float distance, bool moveInWorldPlane);
    void moveRight(float distance, bool moveInWorldPlane);
    void moveUp(float distance);
    void moveToTarget(float amt);
private:
    bool _viewProjMatrixUpdated;
    bool _isLerping;
    int _lerpSteps;
    int _hash;

    Vector3 _lerpTarget;
    Vector3 _lerpPos;
    Vector3 _lerpUp;

    void generateFrustum();
    void updateViewProjMatrix();
};

#endif
