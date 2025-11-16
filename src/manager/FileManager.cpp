#pragma once

#include <string>
#include <vector>

#include "../object/ObjectSpec.cpp"

class FileManager {
public:
    std::vector<ObjectSpec> read(const std::string fileName) {
        std::vector<ObjectSpec> objectSpecs;
        
        objectSpecs.push_back({"Wall", {1.0, 1.0, 1.0}, {0.0, 0.5, -19.5}, {40.0, 1.0, 1.0}});
        objectSpecs.push_back({"Wall", {1.0, 1.0, 1.0}, {0.0, 0.5, 19.5}, {40.0, 1.0, 1.0}});
        objectSpecs.push_back({"Wall", {1.0, 1.0, 1.0}, {-19.5, 0.5, 0.0}, {1.0, 1.0, 40.0}});
        objectSpecs.push_back({"Wall", {1.0, 1.0, 1.0}, {19.5, 0.5, 0.0}, {1.0, 1.0, 40.0}});
        objectSpecs.push_back({"Floor", {0.8, 0.8, 0.8}, {0.0, -0.5, 0.0}, {40.0, 1.0, 40.0}});

        // Tree
        objectSpecs.push_back({"Wall", {0.43, 0.2, 0.15}, {10.0, 2.5, -5.0}, {1.0, 5.0, 1.0}});
        objectSpecs.push_back({"Wall", {0.0, 0.8, 0.3}, {10.0, 5 + 0.5, -5.0}, {4.0, 1.0, 4.0}});
        objectSpecs.push_back({"Wall", {0.0, 0.8, 0.3}, {10.0, 5 + 1.5, -5.0}, {3.0, 1.0, 3.0}});
        objectSpecs.push_back({"Wall", {0.0, 0.8, 0.3}, {10.0, 5 + 2.5, -5.0}, {2.0, 1.0, 2.0}});
        
        // Rock
        objectSpecs.push_back({"Wall", {0.2, 0.2, 0.2}, {-15.0, 1.0, 2.0}, {4.0, 2.0, 4.0}});

        objectSpecs.push_back({"Sphere", {0.8, 0.2, 0.2}, {15.0, 3.0, 15.0}, {0.5, 0.0, 0.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.8, 0.2, 0.2}, {15.0, 3.0, 0.0}, {0.5, 0.0, 0.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.8, 0.2, 0.2}, {15.0, 3.0, -15.0}, {0.5, 0.0, 0.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.8, 0.2, 0.2}, {-15.0, 3.0, 15.0}, {0.5, 0.0, 0.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.8, 0.2, 0.2}, {-15.0, 3.0, 0.0}, {0.5, 0.0, 0.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.8, 0.2, 0.2}, {-15.0, 3.0, -15.0}, {0.5, 0.0, 0.0}, {0.0, 0.0, 0.0}});


        objectSpecs.push_back({"Cannon", {0.3, 0.3, 0.3}, {0.0, 20.0, -35.0}, {1.0, 1.0, 5.0}});

        return objectSpecs;
    }
};
