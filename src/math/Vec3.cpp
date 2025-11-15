#pragma once

#include <GLUT/glut.h>

#pragma once

struct Vec3 {
    double x;
    double y;
    double z;

    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
};

double dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
Vec3 operator*(const double a, const Vec3& b) { return {a * b.x, a * b.y, a * b.z}; }

GLfloat BoxVerts[][3] = {
    {-1.0,-1.0,-1.0},
    {-1.0,-1.0,1.0},
    {-1.0,1.0,-1.0},
    {-1.0,1.0,1.0},
    {1.0,-1.0,-1.0},
    {1.0,-1.0,1.0},
    {1.0,1.0,-1.0},
    {1.0,1.0,1.0}
};

GLfloat bNorms[][3] = {
    {-1.0,0.0,0.0},
    {1.0,0.0,0.0},
    {0.0,1.0,0.0},
    {0.0,-1.0,0.0},
    {0.0,0.0,-1.0},
    {0.0,0.0,1.0}
};

int cubeIndices[][4] = {
    {0,1,3,2},
    {4,6,7,5},
    {2,3,7,6},
    {0,4,5,1},
    {0,2,6,4},
    {1,5,7,3}
};
