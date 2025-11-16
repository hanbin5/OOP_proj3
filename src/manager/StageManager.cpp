#pragma once

#include <memory>
#include <string>

#include <GLUT/glut.h>

#include "ObjectManager.cpp"
#include "RenderManager.cpp"
#include "PhysicsManager.cpp"
#include "FileManager.cpp"
#include "TimeManager.cpp"
#include "CannonManager.cpp"
#include "../object/Cannon.cpp"

class StageManager {
public:
    StageManager(FileManager& files,
                 RenderManager& renderer)
        : fileManager(files),
          renderManager(renderer) {}

    void loadStage(const std::string& stageName) {
        cannonManager.reset();
        objectManager = std::make_unique<ObjectManager>();
        physicsManager = std::make_unique<PhysicsManager>();
        timeManager = std::make_unique<TimeManager>();
        objectManager->init(fileManager.read(stageName));
        if (Cannon* cannon = findCannon()) {
            cannon->center = {0.0, 20.0, -35.0};
            cannon->velocity = {0.0, 0.0, 0.0};
            cannon->accel = {0.0, 0.0, 0.0};
            cannonManager = std::make_unique<CannonManager>(*cannon);
            cannonManager->update(0.0f);
        }
        timeManager->reset();
        syncCamera();
    }

    void update() {
        if (!objectManager || !physicsManager || !timeManager) return;
        double delta = timeManager->tick();
        if (delta <= 0.0) delta = 0.016;
        physicsManager->update(*objectManager, delta);
        if (cannonManager) {
            cannonManager->update(static_cast<float>(delta));
        }
        syncCamera();
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
        if (cannonManager) {
            cannonManager->onKeyboard(key, pressed);
        }
    }

    void onSpecial(int, int, int, bool) {}

private:
    FileManager& fileManager;
    RenderManager& renderManager;
    std::unique_ptr<ObjectManager> objectManager;
    std::unique_ptr<PhysicsManager> physicsManager;
    std::unique_ptr<TimeManager> timeManager;
    std::unique_ptr<CannonManager> cannonManager;

    void syncCamera() {
        if (cannonManager) {
            renderManager.setCamera(cannonManager->getCamera());
        }
    }

    Cannon* findCannon() {
        if (!objectManager) return nullptr;
        for (auto& objPtr : objectManager->getObjectList()) {
            if (!objPtr) continue;
            if (auto cannonPtr = dynamic_cast<Cannon*>(objPtr.get())) {
                return cannonPtr;
            }
        }
        return nullptr;
    }
};
