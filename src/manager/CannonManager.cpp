#pragma once

#include <cmath>

#include <GLUT/glut.h>

#include "ObjectManager.cpp"
#include "../object/Cannon.cpp"
#include "../object/TransWall.cpp"

#include "../math/Rotate.cpp"

class CannonManager {
public:
    void init(ObjectManager* objManager) {
        objectManager = objManager;
        cannon = findCannon();

        ObjectSpec spec{};
        spec.objectName = "TransWall";
        spec.rgb = {0.6, 0.2, 0.2};
        spec.center = cannon->center;
        spec.size = {0.1, 0.1, 40.0};
        auto red_line = objectManager->makeObject(spec);
        redLine = dynamic_cast<TransWall*>(red_line.get());
        objectManager->getObjectList().push(std::move(red_line));
        if (redLine) {
            postMultiplyTranslate(redLine->m_mRotate,
                                  redLineLocalOffset.x,
                                  redLineLocalOffset.y,
                                  redLineLocalOffset.z);
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

    void rotateUp() {
        if (!cannon) return;
        postMultiplyPitch(cannon->m_mRotate, 1);
        cannon->setDirection();
        rotateRedLineAroundOffset(0.0, 1.0);
        syncRedLineCenter();
    }
    void rotateDown() {
        if (!cannon) return;
        postMultiplyPitch(cannon->m_mRotate, -1);
        cannon->setDirection();
        rotateRedLineAroundOffset(0.0, -1.0);
        syncRedLineCenter();
    }
    void rotateLeft() {
        if (!cannon) return;
        postMultiplyYaw(cannon->m_mRotate, -1);
        cannon->setDirection();
        rotateRedLineAroundOffset(-1.0, 0.0);
        syncRedLineCenter();
    }
    void rotateRight() {
        if (!cannon) return;
        postMultiplyYaw(cannon->m_mRotate, 1);
        cannon->setDirection();
        rotateRedLineAroundOffset(1.0, 0.0);
        syncRedLineCenter();
    }
    
    void moveForward() {
        if (!cannon) return;
        cannon->center.x += 1;
        syncRedLineCenter();
    }
    void moveBackward() {
        if (!cannon) return;
        cannon->center.x -= 1;
        syncRedLineCenter();
    }
    void moveLeft() {
        if (!cannon) return;
        cannon->center.z += 1;
        syncRedLineCenter();
    }
    void moveRight() {
        if (!cannon) return;
        cannon->center.z -= 1;
        syncRedLineCenter();
    }

    void fire() {
        if (!objectManager || !cannon) return;

        constexpr double projectileSpeed = 30.0;
        ObjectSpec spec{};
        spec.objectName = "Sphere";
        spec.rgb = {0.8, 0.2, 0.2};
        spec.center = cannon->center;
        spec.size = {0.5, 0.5, 0.5};
        const Vec3 direction = cannon->getDirection();
        spec.velocity = {
            direction.x * projectileSpeed,
            direction.y * projectileSpeed,
            direction.z * projectileSpeed
        };

        auto projectile = objectManager->makeObject(spec);
        objectManager->getObjectList().push(std::move(projectile));
    }

    double getX() { return cannon->center.x; }
    double getY() { return cannon->center.y; }
    double getZ() { return cannon->center.z; }
    double getYaw() { return 0; }
    double getPitch() { return -80.0; }

private:
    void rotateRedLineAroundOffset(double yawDeg, double pitchDeg) {
        if (!redLine) return;
        postMultiplyRotateAroundOffset(redLine->m_mRotate,
                                       redLineLocalOffset,
                                       yawDeg,
                                       pitchDeg);
    }

    void syncRedLineCenter() {
        if (!redLine || !cannon) return;
        redLine->center = cannon->center;
    }

    ObjectManager* objectManager;
    Cannon* cannon;
    TransWall* redLine = nullptr;
    const Vec3 redLineLocalOffset{0.0, 0.0, 20.0};
};
