#pragma once

#include <cmath>

#include <GLUT/glut.h>

#include "../math/Vec3.cpp"

class Object
{
public:
    GLdouble m_mRotate[16];
    Vec3 center;
    Vec3 color;
    Vec3 velocity;
    Vec3 accel;
    Vec3 size;
    double mass;

    virtual ~Object() = default;

    virtual void draw() = 0;

    void init() {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glGetDoublev(GL_MODELVIEW_MATRIX, m_mRotate);
        glPopMatrix();
    }

    void setColor(double r, double g, double b) {
        color = {r, g, b};
    }
    void setColor(Vec3 rgb) {
        color = rgb;
    }

    void setCenter(Vec3 new_loc) {
        center = new_loc;
    }
    void setCenter(double x, double y, double z) {
        center.x = x; center.y = y; center.z = z;
    }

    void setSize(Vec3 new_size) {
        size = new_size;
    }
    
    void setVelocity(Vec3 new_velocity) {
        velocity = new_velocity;
    }

    void acc() {
        velocity = velocity + accel;
    }

    virtual bool hasIntersected(Object& obj) const = 0;

    virtual void hitBy(Object& obj) {
        if (&obj == this) return;
        if (!hasIntersected(obj)) return;

        Vec3 n = center - obj.center;
        double dist2 = dot(n, n);
        if (dist2 < 1e-12) return;
        double invLen = 1.0 / std::sqrt(dist2);
        n = { n.x * invLen, n.y * invLen, n.z * invLen };

        Vec3 v_rel = velocity - obj.velocity;
        double vrelN = dot(v_rel, n);
        if (vrelN >= 0.0) return;

        double e = 0.5;
        double invM1 = (mass > 0) ? 1.0 / mass : 0.0;
        double invM2 = (obj.mass > 0) ? 1.0 / obj.mass : 0.0;

        double j = -(1.0 + e) * vrelN / (invM1 + invM2);
        
        velocity = velocity + (j * invM1) * n;
        obj.velocity = obj.velocity - (j * invM2) * n;
    }
};
