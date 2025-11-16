#pragma once

#include <vector>

#include "ObjectManager.cpp"

class PhysicsManager {
public:
    void update(ObjectManager& objectManager, double deltaTime) {
        auto& objectList = objectManager.getObjectList();
        std::vector<Object*> activeObjects;
        activeObjects.reserve(objectList.size());

        for (const auto& objPtr : objectList) {
            if (!objPtr) continue;

            // Semi-implicit Euler integration
            objPtr->velocity = objPtr->velocity + deltaTime * objPtr->accel;
            objPtr->center = objPtr->center + deltaTime * objPtr->velocity;

            activeObjects.push_back(objPtr.get());
        }

        const size_t count = activeObjects.size();
        for (size_t i = 0; i < count; ++i) {
            for (size_t j = i + 1; j < count; ++j) {
                activeObjects[i]->hitBy(*activeObjects[j]);
            }
        }
    }
};
