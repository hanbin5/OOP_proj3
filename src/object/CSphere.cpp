#pragma once

#include "Object.cpp"

class CSphere : public Object 
{
public:
    double radius;

    CSphere(double radius = 0.5) {
        this->radius = radius;
        center = {0, 0, 0};
        color = {0, 0, 0};
        velocity = {0, 0, 0};
        accel = {0, -0.098, 0};

        mass = std::pow(radius, 3) * (4.0/3.0) * 3.14;
    }

    void draw()
    {
        glColor3f(color.x, color.y, color.z);
        glutSolidSphere(radius, 20, 20);
    }

    bool hasIntersected(Object& obj) const override {
        auto ball = dynamic_cast<const CSphere*>(&obj);
        if (!ball) return false;

        double dx = center.x - ball->center.x;
        double dy = center.y - ball->center.y;
        double dz = center.z - ball->center.z;
        double rsum = radius + ball->radius;
        return (dx*dx + dy*dy + dz*dz) <= (rsum * rsum);
    }
};
