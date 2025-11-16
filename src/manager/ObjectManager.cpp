#pragma once

#include <memory>
#include <stdexcept>
#include <utility>

#include "../object/ObjectList.cpp"
#include "../object/ObjectSpec.cpp"
#include "../object/Sphere.cpp"
#include "../object/Floor.cpp"
#include "../object/Wall.cpp"
#include "../object/Cannon.cpp"
#include "../object/TransWall.cpp"

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
            auto sphere = std::make_unique<Sphere>(objectSpec);
            object = std::move(sphere);
        } else if (objectSpec.objectName == "Floor") {
            auto floor = std::make_unique<Floor>(objectSpec);
            object = std::move(floor);
        } else if (objectSpec.objectName == "Wall") {
            auto wall = std::make_unique<Wall>(objectSpec);
            object = std::move(wall);
        } else if (objectSpec.objectName == "TransWall") {
            auto wall = std::make_unique<TransWall>(objectSpec);
            object = std::move(wall);
        } else if (objectSpec.objectName == "Cannon") {
            auto cannon = std::make_unique<Cannon>(objectSpec);
            object = std::move(cannon);
        } else {
            throw std::runtime_error("Unknown object type: " + objectSpec.objectName);
        }

        object->init();
        return object;
    }
};
