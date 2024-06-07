#include <algorithm>
#include <numeric>

#include "camera.h"
#include "../constants.h"
#include "../../interface/EventConsumer.h"
#include "../../interface/FrameTimeAvg.h"
#include "../../util/math.h"
#include "../../interface/settings/data/SettingsData.h"

constexpr bool moveInWorldPlane = true;
constexpr bool rotateAroundTarget = true;
constexpr bool lockView = true;
constexpr bool rotateUp = false;

bool RenderCamera::sphereOutsideFrustum(float x, float y, float z, float r) {
    // Update cache if invalidated
    if (!_viewProjMatrixUpdated) {
        updateViewProjMatrix();
        generateFrustum();
        _viewProjMatrixUpdated = true;
    }

    // If distance to plane is "below" the plane and more than
    // the radius of the sphere, the sphere is 100% below the plane
    // (outside the frustum, which is made of 6 planes)
    for (const auto &plane : frustum) {
        if (plane.distanceToPlane(x, y, z) < -r)
            return true;
    }
    return false;
}

void RenderCamera::update() {
    updateLerp();
    const float deltaAvg = settings::data::ref()->ui->frameIndependentCam ?
        FrameTime::ref()->getDelta() :
        2.0f / TARGET_FPS;

    auto controlScheme = settings::data::ref()->ui->movementMode;
    if (controlScheme == settings::UI::MovementMode::FIRST_PERSON)
        updateControlsFirstPerson(deltaAvg);
    else if (controlScheme == settings::UI::MovementMode::THREED)
        updateControls3DEditor(deltaAvg);

    updateControlsShared(deltaAvg);
}

void RenderCamera::updateControlsFirstPerson(const float delta) {
    Vector2 mouseDelta = EventConsumer::ref()->getMouseDelta() * settings::data::ref()->ui->mouseSensitivity;
    const float dm = delta * TARGET_FPS;

    // Keyboard WASD to move
    if (EventConsumer::ref()->isKeyDown(KEY_W))
        moveForward(CAMERA_MOVE_SPEED * dm, moveInWorldPlane);
    if (EventConsumer::ref()->isKeyDown(KEY_A))
        moveRight(-CAMERA_MOVE_SPEED * dm, moveInWorldPlane);
    if (EventConsumer::ref()->isKeyDown(KEY_S))
        moveForward(-CAMERA_MOVE_SPEED * dm, moveInWorldPlane);
    if (EventConsumer::ref()->isKeyDown(KEY_D))
        moveRight(CAMERA_MOVE_SPEED * dm, moveInWorldPlane);
    if (EventConsumer::ref()->isKeyDown(KEY_SPACE))
        moveUp(CAMERA_MOVE_SPEED * dm);
    if (EventConsumer::ref()->isKeyDown(KEY_LEFT_SHIFT))
        moveUp(-CAMERA_MOVE_SPEED * dm);

    // Look where pointing (press r mouse btn to toggle)
    if (EventConsumer::ref()->isMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        _first_person_locked = !_first_person_locked;

    if (_first_person_locked && (mouseDelta.x || mouseDelta.y)) {
        Vector3 targetPrime = camera.target - camera.position;

        // Rotate point in XZ plane (looking left/right)
        if (mouseDelta.x) {
            const float theta = CAMERA_FIRST_PERSON_ROTATION_SPEED * mouseDelta.x;
            camera.target.x = targetPrime.x * std::cos(theta) - targetPrime.z * std::sin(theta) + camera.position.x;
            camera.target.z = targetPrime.z * std::cos(theta) + targetPrime.x * std::sin(theta) + camera.position.z;
            targetPrime = camera.target - camera.position;
        }
        // Rotate vertically (XZ / Y) plane
        if (mouseDelta.y) {
            const float theta = -CAMERA_FIRST_PERSON_ROTATION_SPEED * mouseDelta.y;

            // Project the current target onto a XZ plane with the same y level as
            // the current camera position, this is distance from camera to the projected point
            // This is our pseudo "x" for rotation
            const float XZDistance = Vector3Distance(camera.position,
                Vector3{ camera.target.x, camera.position.y, camera.target.z });
            // If the new distance is right below / above us the camera gets all glitchy since we use Euler angles
            const float newXZDistance = std::max(3.0f, XZDistance * std::cos(theta) - targetPrime.y * std::sin(theta));
            camera.target.y = targetPrime.y * std::cos(theta) + XZDistance * std::sin(theta) + camera.position.y;

            Vector3 originalXZVec = Vector3Normalize(Vector3{ targetPrime.x, 0.0f, targetPrime.z });
            camera.target.x = camera.position.x + originalXZVec.x * newXZDistance;
            camera.target.z = camera.position.z + originalXZVec.z * newXZDistance;
        }

        SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
        invalidateCache();
    }
}


void RenderCamera::updateControls3DEditor(const float delta) {
    const Vector2 mouseDelta = EventConsumer::ref()->getMouseDelta() * settings::data::ref()->ui->mouseSensitivity;
    const float dm = delta * TARGET_FPS;

    // RMouse drag to rotate
    if (EventConsumer::ref()->isMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        pitch(-CAMERA_MOUSE_ROTATION_SPEED * mouseDelta.y * dm, lockView, rotateAroundTarget, rotateUp);
        yaw(-CAMERA_MOUSE_ROTATION_SPEED * mouseDelta.x * dm, rotateAroundTarget);
    }
}


void RenderCamera::updateControlsShared(const float delta) {
    const Vector2 mouseDelta = EventConsumer::ref()->getMouseDelta() * settings::data::ref()->ui->mouseSensitivity;
    const float dm = delta * TARGET_FPS;

    // Hold middle to pan
    if (EventConsumer::ref()->isMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        const float deltaScaleFactor = GetScreenWidth() / 60.0f; // Arbritrary scaling for how much pan affects
        const Vector3 rightDir = GetCameraRight(&camera) * -mouseDelta.x / deltaScaleFactor;

        // Raylib's GetCameraUp gets up direction for rotation, not the 2D "up"
        // direction on your screen for panning, so we have to do cross product here
        const Vector3 trueUp = Vector3Normalize(
                Vector3CrossProduct(GetCameraRight(&camera), GetCameraForward(&camera)));
        const Vector3 upDir = trueUp * mouseDelta.y / deltaScaleFactor;
        const Vector3 posDelta = (rightDir + upDir) * (CAMERA_MOVE_SPEED_PAN * dm);

        camera.position += posDelta;
        camera.target += posDelta;

        if (mouseDelta.x || mouseDelta.y)
            invalidateCache();
    }

    // Keyboard camera rotation
    if (EventConsumer::ref()->isKeyDown(KEY_DOWN))
        pitch(-CAMERA_ROTATION_SPEED * dm, lockView, rotateAroundTarget, rotateUp);
    if (EventConsumer::ref()->isKeyDown(KEY_UP))
        pitch(CAMERA_ROTATION_SPEED * dm, lockView, rotateAroundTarget, rotateUp);
    if (EventConsumer::ref()->isKeyDown(KEY_RIGHT))
        yaw(-CAMERA_ROTATION_SPEED * dm, rotateAroundTarget);
    if (EventConsumer::ref()->isKeyDown(KEY_LEFT))
        yaw(CAMERA_ROTATION_SPEED * dm, rotateAroundTarget);
    if (EventConsumer::ref()->isKeyDown(KEY_Q))
        roll(-CAMERA_ROTATION_SPEED * dm);
    if (EventConsumer::ref()->isKeyDown(KEY_E))
        roll(CAMERA_ROTATION_SPEED * dm);

    // Scroll to zoom in / out
    moveToTarget(-CAMERA_MOUSE_ZOOM_SPEED * dm * EventConsumer::ref()->getMouseWheelMove());
    if (EventConsumer::ref()->isKeyDown(KEY_KP_SUBTRACT))
        moveToTarget(CAMERA_ZOOM_SPEED * dm);
    if (EventConsumer::ref()->isKeyDown(KEY_KP_ADD))
        moveToTarget(-CAMERA_ZOOM_SPEED * dm);
}


void RenderCamera::updateLerp() {
    // Update lerp
    if (_isLerping) {
        constexpr float MAX_ERR = 0.001f;
        constexpr int MAX_LERP_STEPS = 20; // If lerping doesn't converge just go straight to end
        const float LERP_SPEED = _lerpSteps < MAX_LERP_STEPS / 4 ? 0.35f : 0.9f; // lerp faster as time goes on

        _lerpSteps++;
        camera.position.x = Lerp(camera.position.x, _lerpPos.x, LERP_SPEED);
        camera.position.y = Lerp(camera.position.y, _lerpPos.y, LERP_SPEED);
        camera.position.z = Lerp(camera.position.z, _lerpPos.z, LERP_SPEED);
        camera.target.x = Lerp(camera.target.x, _lerpTarget.x, LERP_SPEED);
        camera.target.y = Lerp(camera.target.y, _lerpTarget.y, LERP_SPEED);
        camera.target.z = Lerp(camera.target.z, _lerpTarget.z, LERP_SPEED);
        camera.up.x = Lerp(camera.up.x, _lerpUp.x, LERP_SPEED);
        camera.up.y = Lerp(camera.up.y, _lerpUp.y, LERP_SPEED);
        camera.up.z = Lerp(camera.up.z, _lerpUp.z, LERP_SPEED);
        invalidateCache();

        if (_lerpSteps > MAX_LERP_STEPS || (
                util::vec3_similar(camera.position, _lerpPos, MAX_ERR) &&
                util::vec3_similar(camera.target, _lerpTarget, MAX_ERR) &&
                util::vec3_similar(camera.up, _lerpUp, MAX_ERR)
            )) {
            _isLerping = false;
            _lerpSteps = 0;

            camera.position = _lerpPos;
            camera.target = _lerpTarget;
            camera.up = _lerpUp;
        }
    }
}


// Logic for this code is from
// https://gdbooks.gitbooks.io/3dcollisions/content/Chapter6/frustum.html
// Another useful resource: https://www.flipcode.com/archives/Frustum_Culling.shtml
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
    const Matrix matProj = MatrixPerspective(camera.fovy * DEG2RAD, aspectRatio,
        RL_CULL_DISTANCE_NEAR, RL_CULL_DISTANCE_FAR);
    const Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);
    viewProjMatrix = MatrixMultiply(matView, matProj);
}

void RenderCamera::moveUp(float distance) {
    invalidateCache();
    Vector3 up = GetCameraUp(&camera);
    up = Vector3Scale(up, distance);

    Vector3 targetPos = camera.position + up;
    if (boundError(targetPos) > boundError(camera.position)) return;

    camera.position = targetPos;
    camera.target = camera.target + up;
}

void RenderCamera::moveForward(float distance, bool moveInWorldPlane = true) {
    invalidateCache();
    Vector3 forward = GetCameraForward(&camera);
    if (moveInWorldPlane) {
        forward.y = 0;
        forward = Vector3Normalize(forward);
    }
    forward = Vector3Scale(forward, distance);

    Vector3 targetPos = camera.position + forward;
    if (boundError(targetPos) > boundError(camera.position)) return;

    camera.position = targetPos;
    camera.target = camera.target + forward;
}

void RenderCamera::moveRight(float distance, bool moveInWorldPlane = true) {
    invalidateCache();
    Vector3 right = GetCameraRight(&camera);

    if (moveInWorldPlane) {
        right.y = 0;
        right = Vector3Normalize(right);
    }
    right = Vector3Scale(right, distance);

    Vector3 targetPos = camera.position + right;
    if (boundError(targetPos) > boundError(camera.position)) return;

    camera.position = targetPos;
    camera.target = camera.target + right;
}

void RenderCamera::moveToTarget(float amt) {
    if (amt == 0.0f) return;
    invalidateCache();

    float distance = Vector3Distance(camera.position, camera.target);
    distance = std::max(0.1f, distance + amt);

    Vector3 forward = GetCameraForward(&camera);
    Vector3 targetPos = camera.target + forward * -distance;
    if (boundError(targetPos) > boundError(camera.position)) return;
    camera.position = targetPos;
}
