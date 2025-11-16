#pragma once

#include <string>
#include <vector>

#include "../object/ObjectSpec.cpp"

class FileManager {
public:
    std::vector<ObjectSpec> read(const std::string) {
        std::vector<ObjectSpec> objectSpecs;

        // Arena bounds
        // objectSpecs.push_back({"Wall", {1.0, 1.0, 1.0}, {0.0, 0.5, -19.5}, {40.0, 1.0, 1.0}});
        // objectSpecs.push_back({"Wall", {1.0, 1.0, 1.0}, {0.0, 0.5, 19.5}, {40.0, 1.0, 1.0}});
        // objectSpecs.push_back({"Wall", {1.0, 1.0, 1.0}, {-19.5, 0.5, 0.0}, {1.0, 1.0, 40.0}});
        // objectSpecs.push_back({"Wall", {1.0, 1.0, 1.0}, {19.5, 0.5, 0.0}, {1.0, 1.0, 40.0}});
        objectSpecs.push_back({"Floor", {0.75, 0.95, 0.75}, {0.0, -0.5, 0.0}, {40.0, 1.0, 40.0}});

        // Decorative structures
        objectSpecs.push_back({"Wall", {0.45, 0.45, 0.45}, {0.0, 2.0, 0.0}, {6.0, 4.0, 6.0}});
        // objectSpecs.push_back({"Wall", {0.4, 0.2, 0.1}, {-10.0, 2.0, 8.0}, {2.0, 4.0, 2.0}});
        // objectSpecs.push_back({"Wall", {0.4, 0.2, 0.1}, {10.0, 3.0, -8.0}, {2.0, 6.0, 2.0}});
        // objectSpecs.push_back({"Wall", {0.3, 0.3, 0.3}, {0.0, 1.5, -12.0}, {8.0, 3.0, 2.0}});
        // objectSpecs.push_back({"Wall", {0.3, 0.3, 0.3}, {-8.0, 1.5, 0.0}, {2.0, 3.0, 8.0}});

        // Ball
        objectSpecs.push_back({"Sphere", {0.95, 0.2, 0.2}, {-12.0, 14.0, -12.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.95, 0.6, 0.2}, {12.0, 5.0, -10.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.2, 0.6, 0.95}, {-12.0, 6.0, -12.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.2, 0.95, 0.5}, {10.0, 7.0, 10.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.9, 0.3, 0.9}, {0.0, 6.5, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.95, 0.85, 0.2}, {0.0, 8.0, -8.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}});
        objectSpecs.push_back({"Sphere", {0.25, 0.85, 0.85}, {0.0, 9.0, 8.0}, {1.2, 0.0, 0.0}, {0.0, 0.0, 0.0}});

        // Canon
        objectSpecs.push_back({"Cannon", {0.3, 0.3, 0.3}, {0.0, 20.0, 0.0}, {1.0, 1.0, 5.0}});

        return objectSpecs;
    }
};
