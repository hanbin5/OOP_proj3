#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <memory>
#include <string>

#include <GLUT/glut.h>

#include "ObjectManager.cpp"
#include "RenderManager.cpp"
#include "PhysicsManager.cpp"
#include "FileManager.cpp"
#include "TimeManager.cpp"

class StageManager {
public:
    StageManager(FileManager& files,
                 RenderManager& renderer)
        : fileManager(files),
          renderManager(renderer) {}

    void loadStage(const std::string& stageName) {
        objectManager = std::make_unique<ObjectManager>();
        physicsManager = std::make_unique<PhysicsManager>();
        timeManager = std::make_unique<TimeManager>();
        objectManager->init(fileManager.read(stageName));
        timeManager->reset();
        syncCamera();
    }

    void update() {
        if (!objectManager || !physicsManager || !timeManager) return;
        double delta = timeManager->tick();
        if (delta <= 0.0) delta = 0.016;
        physicsManager->update(*objectManager, delta);
        updateCamera(static_cast<float>(delta));
    }

    void render() {
        if (!objectManager) return;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderManager.drawAll(*objectManager);
        glutSwapBuffers();
    }

    void onResize(int width, int height) {
        renderManager.resize(width, height);
    }

    void onKeyboard(unsigned char key, int, int, bool pressed) {
        const unsigned char lower = static_cast<unsigned char>(std::tolower(key));
        if (lower == 'w') cameraInput.moveForward = pressed;
        else if (lower == 's') cameraInput.moveBackward = pressed;
        else if (lower == 'a') cameraInput.moveLeft = pressed;
        else if (lower == 'd') cameraInput.moveRight = pressed;
    }

    void onSpecial(int key, int, int, bool pressed) {
        if (key == GLUT_KEY_UP) cameraInput.pitchUp = pressed;
        else if (key == GLUT_KEY_DOWN) cameraInput.pitchDown = pressed;
        else if (key == GLUT_KEY_LEFT) cameraInput.yawLeft = pressed;
        else if (key == GLUT_KEY_RIGHT) cameraInput.yawRight = pressed;
    }

private:
    FileManager& fileManager;
    RenderManager& renderManager;
    std::unique_ptr<ObjectManager> objectManager;
    std::unique_ptr<PhysicsManager> physicsManager;
    std::unique_ptr<TimeManager> timeManager;
    CameraState camera{0.0f, 0.0f, 10.0f, 0.0f, 0.0f};
    const float rotationSpeed = 5.0f;
    const float moveSpeed = 0.5f;
    struct CameraInput {
        bool yawLeft = false;
        bool yawRight = false;
        bool pitchUp = false;
        bool pitchDown = false;
        bool moveForward = false;
        bool moveBackward = false;
        bool moveLeft = false;
        bool moveRight = false;
    } cameraInput;

    void syncCamera() {
        renderManager.setCamera(camera);
    }

    void updateCamera(float deltaTime) {
        bool changed = false;
        if (cameraInput.yawLeft) { adjustYaw(-rotationSpeed * deltaTime * 10.0f); changed = true; }
        if (cameraInput.yawRight) { adjustYaw(rotationSpeed * deltaTime * 10.0f); changed = true; }
        if (cameraInput.pitchUp) { adjustPitch(rotationSpeed * deltaTime * 10.0f); changed = true; }
        if (cameraInput.pitchDown) { adjustPitch(-rotationSpeed * deltaTime * 10.0f); changed = true; }
        if (cameraInput.moveForward) { translateForward(moveSpeed * deltaTime * 10.0f); changed = true; }
        if (cameraInput.moveBackward) { translateForward(-moveSpeed * deltaTime * 10.0f); changed = true; }
        if (cameraInput.moveLeft) { translateRight(-moveSpeed * deltaTime * 10.0f); changed = true; }
        if (cameraInput.moveRight) { translateRight(moveSpeed * deltaTime * 10.0f); changed = true; }
        if (changed) syncCamera();
    }

    void adjustYaw(float delta) {
        camera.yaw += delta;
    }

    void adjustPitch(float delta) {
        camera.pitch = std::clamp(camera.pitch + delta, -89.0f, 89.0f);
    }

    void translateForward(float distance) {
        const float yawRad = degreesToRadians(camera.yaw);
        const float pitchRad = degreesToRadians(camera.pitch);
        const float cosPitch = std::cos(pitchRad);
        const float forwardX = cosPitch * std::sin(yawRad);
        const float forwardY = std::sin(pitchRad);
        const float forwardZ = cosPitch * std::cos(yawRad);
        camera.x += forwardX * distance;
        camera.y += forwardY * distance;
        camera.z += forwardZ * distance;
    }

    void translateRight(float distance) {
        const float yawRad = degreesToRadians(camera.yaw);
        const float rightX = std::cos(yawRad);
        const float rightZ = -std::sin(yawRad);
        camera.x += rightX * distance;
        camera.z += rightZ * distance;
    }

    static float degreesToRadians(float degrees) {
        return degrees * static_cast<float>(M_PI) / 180.0f;
    }
};
