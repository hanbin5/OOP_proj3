#pragma once

#include <memory>
#include <stdexcept>
#include <utility>

#include "../object/ObjectList.cpp"
#include "../object/ObjectSpec.cpp"
#include "../object/CSphere.cpp"
#include "../object/CFloor.cpp"
#include "../object/CWall.cpp"

class ObjectManager {
private:
    ObjectList objectList;

public:
    ObjectList& getObjectList() {
        return objectList;
    }

    const ObjectList& getObjectList() const {
        return objectList;
    }

    void init(const std::vector<ObjectSpec>& objectSpecs) {
        objectList.reset();

        for (const auto& spec : objectSpecs) {
            objectList.push(makeObject(spec));
        }
    }

    std::unique_ptr<Object> makeObject(const ObjectSpec& objectSpec) {
        std::unique_ptr<Object> object;

        if (objectSpec.objectName == "Ship" || objectSpec.objectName == "Sphere") {
            auto sphere = std::make_unique<CSphere>();
            sphere->setColor(objectSpec.rgb.x, objectSpec.rgb.y, objectSpec.rgb.z);
            sphere->setCenter(objectSpec.center.x, objectSpec.center.y, objectSpec.center.z);
            sphere->velocity = objectSpec.velocity;
            object = std::move(sphere);
        } else if (objectSpec.objectName == "Floor") {
            auto floor = std::make_unique<CFloor>();
            floor->setColor(objectSpec.rgb.x, objectSpec.rgb.y, objectSpec.rgb.z);
            floor->setCenter(objectSpec.center.x, objectSpec.center.y, objectSpec.center.z);
            floor->velocity = objectSpec.velocity;
            object = std::move(floor);
        } else if (objectSpec.objectName == "Wall") {
            auto wall = std::make_unique<CWall>();
            wall->setColor(objectSpec.rgb.x, objectSpec.rgb.y, objectSpec.rgb.z);
            wall->setCenter(objectSpec.center.x, objectSpec.center.y, objectSpec.center.z);
            wall->velocity = objectSpec.velocity;
            object = std::move(wall);
        } else {
            throw std::runtime_error("Unknown object type: " + objectSpec.objectName);
        }

        object->init();
        return object;
    }
};
