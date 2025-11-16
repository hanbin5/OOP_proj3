#pragma once

#include "CannonManager.cpp"

struct CameraState {
    double x = 0.0f;
    double y = 0.0f;
    double z = 10.0f;
    double yaw = 0.0f;
    double pitch = 0.0f;
};

class CameraManager {
public:
    CameraState getCamera() { return camera; }
    void setCamera(const CameraState& newCamera) {
        camera = newCamera;
    }
    float getPitch() { return camera.pitch; }
    float getYaw() { return camera.yaw; }

    float getX() { return camera.x; }
    float getY() { return camera.y; }
    float getZ() { return camera.z; }

    void update(CannonManager* cannonManger) {
        if (!cannonManger) return;

        CameraState new_state = {
            cannonManger->getX(),
            cannonManger->getY() + 3,
            cannonManger->getZ() - 2,
            cannonManger->getYaw(),
            cannonManger->getPitch()
        };
        camera = new_state;
    }

private:
    CameraState camera;
};
