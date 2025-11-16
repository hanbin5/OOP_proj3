#pragma once

#include <string>

#include "../math/Vec3.cpp"

struct ObjectSpec {
    std::string objectName;
    Vec3 rgb;
    Vec3 center;
    Vec3 size;
    Vec3 velocity;
};
