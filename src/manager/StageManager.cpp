#pragma once

#include <cmath>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include <GLUT/glut.h>

#include "ObjectManager.cpp"
#include "RenderManager.cpp"
#include "PhysicsManager.cpp"
#include "FileManager.cpp"
#include "TimeManager.cpp"
#include "CameraManager.cpp"
#include "CannonManager.cpp"
#include "../object/Cannon.cpp"
#include "../object/Sphere.cpp"

class StageManager {
public:
    StageManager(FileManager& files,
                 RenderManager& renderer)
        : fileManager(files),
          renderManager(renderer) {}

    void loadStage(const std::string& stageName) {
        if (!stageName.empty()) {
            activeStageName = stageName;
        } else if (activeStageName.empty()) {
            activeStageName = "stage1.txt";
        }

        cameraManager = std::make_unique<CameraManager>();
        objectManager = std::make_unique<ObjectManager>();
        physicsManager = std::make_unique<PhysicsManager>();
        timeManager = std::make_unique<TimeManager>();
        cannonManager = std::make_unique<CannonManager>();

        objectManager->init(fileManager.read(activeStageName));
        cannonManager->init(objectManager.get());
        cameraManager->setCamera(createDefaultCamera());
        timeManager->reset();
        remainingShots = maxShots;
        endState = StageEndState::None;
    }

    void update() {
        if (isStageFinished()) {
            checkEndTimer();
            return;
        }
        if (!objectManager || !physicsManager || !timeManager) return;
        double delta = timeManager->tick();
        if (delta <= 0.0) delta = 0.016;

        physicsManager->update(*objectManager, delta);
        cameraManager->update(cannonManager.get());
        checkGameClearCondition();
    }

    void render() {
        if (!objectManager || !cameraManager) return;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderManager.drawAll(*objectManager, *cameraManager);
        drawShotCounterHUD();
        glutSwapBuffers();
    }

    void onResize(int width, int height) {
        viewportWidth = width;
        viewportHeight = height;
        renderManager.resize(width, height);
    }

    void onKeyboard(unsigned char key, int, int, bool pressed) {
        if (!pressed || isStageFinished()) return;

        const unsigned char normalized = static_cast<unsigned char>(std::tolower(key));
        if (normalized == 'w') {
            cannonManager->rotateUp();
        }
        else if (normalized == 's') {
            cannonManager->rotateDown();
        }
        else if (normalized == 'a') {
            cannonManager->rotateLeft();
        }
        else if (normalized == 'd') {
            cannonManager->rotateRight();
        }
        else if (key == ' ') {
            handleFireRequest();
        }
    }

    void onSpecial(int key, int, int, bool pressed) {
        if (!pressed || isStageFinished()) return;

        switch (key) {
            case GLUT_KEY_UP:
                cannonManager->moveRight();
                break;
            case GLUT_KEY_DOWN:
                cannonManager->moveLeft();
                break;
            case GLUT_KEY_LEFT:
                cannonManager->moveBackward();
                break;
            case GLUT_KEY_RIGHT:
                cannonManager->moveForward();
                break;
            default:
                break;
        }
    }

private:
    FileManager& fileManager;
    RenderManager& renderManager;
    std::unique_ptr<CameraManager> cameraManager;
    std::unique_ptr<ObjectManager> objectManager;
    std::unique_ptr<PhysicsManager> physicsManager;
    std::unique_ptr<TimeManager> timeManager;
    std::unique_ptr<CannonManager> cannonManager;
    std::string activeStageName;

    int viewportWidth = 1920;
    int viewportHeight = 1080;
    int remainingShots = 0;
    enum class StageEndState { None, GameOver, GameClear };
    StageEndState endState = StageEndState::None;
    std::chrono::steady_clock::time_point endStateStartTime{};

    void handleFireRequest() {
        if (!cannonManager) return;
        if (remainingShots <= 0) {
            enterGameOverState();
            return;
        }
        cannonManager->fire();
        --remainingShots;
        if (remainingShots <= 0) {
            enterGameOverState();
        }
    }

    bool isStageFinished() const {
        return endState != StageEndState::None;
    }

    void enterGameOverState() {
        if (isStageFinished()) return;
        endState = StageEndState::GameOver;
        std::cout << "Game Over: No shots remaining" << std::endl;
        endStateStartTime = std::chrono::steady_clock::now();
    }

    void enterGameClearState() {
        if (isStageFinished()) return;
        endState = StageEndState::GameClear;
        std::cout << "Game Clear: All spheres removed from floor" << std::endl;
        endStateStartTime = std::chrono::steady_clock::now();
    }

    void checkEndTimer() {
        if (!isStageFinished()) return;
        const auto now = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - endStateStartTime);
        if (elapsed.count() >= 3) {
            std::exit(0);
        }
    }

    void drawShotCounterHUD() const {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, viewportWidth, 0, viewportHeight);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);

        glColor3f(1.0f, 1.0f, 1.0f);
        std::string text = "Shots: " + std::to_string(remainingShots) + "/" + std::to_string(maxShots);
        glRasterPos2f(10.0f, 15.0f);
        for (char c : text) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        if (isStageFinished()) {
            if (endState == StageEndState::GameOver) {
                glColor3f(1.0f, 0.2f, 0.2f);
            } else if (endState == StageEndState::GameClear) {
                glColor3f(0.2f, 1.0f, 0.2f);
            }
            const std::string endText = (endState == StageEndState::GameClear)
                ? "GAME CLEAR"
                : "GAME OVER";
            glRasterPos2f(10.0f, 40.0f);
            for (char c : endText) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
            }
        }

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    void checkGameClearCondition() {
        if (isStageFinished() || !objectManager) return;

        constexpr double floorHalfX = 20.0;
        constexpr double floorHalfZ = 20.0;
        constexpr double floorTopY = 0.0;
        constexpr double epsilon = 0.1;

        bool sphereOnFloor = false;
        for (const auto& objPtr : objectManager->getObjectList()) {
            if (!objPtr) continue;
            auto sphere = dynamic_cast<Sphere*>(objPtr.get());
            if (!sphere) continue;

            const bool withinXZ = std::abs(sphere->center.x) <= floorHalfX + epsilon &&
                                  std::abs(sphere->center.z) <= floorHalfZ + epsilon;
            const double baseY = sphere->center.y - sphere->radius;
            const bool aboveFloor = baseY >= floorTopY - epsilon;
            if (withinXZ && aboveFloor) {
                sphereOnFloor = true;
                break;
            }
        }

        if (!sphereOnFloor) {
            enterGameClearState();
        }
    }

    static CameraState createDefaultCamera() {
        CameraState camera{};
        camera.x = 0.0f;
        camera.y = 30.0f;
        camera.z = 60.0f;
        camera.pitch = -20.0f;
        camera.yaw = 0.0f;
        return camera;
    }

    static constexpr double cannonMoveStep = 1.0;
    static constexpr int maxShots = 10;
};
