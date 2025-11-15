#pragma once

#include <string>
#include <vector>

#include "../object/ObjectSpec.cpp"

class FileManager {
public:
    std::vector<ObjectSpec> read(const std::string fileName) {
        std::vector<ObjectSpec> objectSpecs;

        // simple sandbox objects to ensure the world is populated
        objectSpecs.push_back({"Floor", {0.1, 0.4, 0.6}, {0.0, -3, 0.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.8, 0.2, 0.2}, {3.0, 3.0, 3.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.2, 0.8, 0.2}, {-3.0, 3.0, 3.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.2, 0.2, 0.8}, {-3.0, 3.0, -3.0}, {0.0, 0.0, 0.0}});

        return objectSpecs;
    }
};
