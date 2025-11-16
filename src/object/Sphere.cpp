#pragma once

#include "Object.cpp"
#include "ObjectSpec.cpp"

class Sphere : public Object 
{
public:
    double radius;

    Sphere(ObjectSpec objectSpec) {
        setColor(objectSpec.rgb);
        setCenter(objectSpec.center);
        setSize(objectSpec.size);
        radius = size.x;
        setVelocity(objectSpec.velocity);
        accel = {0, -0.098, 0};

        mass = std::pow(radius, 3) * (4.0/3.0) * 3.14;
    }

    void draw()
    {
        glColor3f(color.x, color.y, color.z);
        glutSolidSphere(radius, 20, 20);
    }

    bool hasIntersected(Object& obj) const override {
        auto ball = dynamic_cast<const Sphere*>(&obj);
        if (!ball) return false;

        double dx = center.x - ball->center.x;
        double dy = center.y - ball->center.y;
        double dz = center.z - ball->center.z;
        double rsum = radius + ball->radius;
        return (dx*dx + dy*dy + dz*dz) <= (rsum * rsum);
    }
};
