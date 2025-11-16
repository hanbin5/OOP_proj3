#pragma once

#include <cmath>

#include "Object.cpp"
#include "ObjectSpec.cpp"
#include "Sphere.cpp"

class Wall : public Object
{

public:
    float width, height, depth;

    GLfloat Verts[8][3];

public :
    Wall(ObjectSpec objectSpec)
    {
        setColor(objectSpec.rgb);
        setCenter(objectSpec.center);
        setVerts(objectSpec.size);
        setVelocity(objectSpec.velocity);
    }

    void setVerts(Vec3 new_size) {
        setSize(new_size);

        int i,j;
        float coef;
        for (i=0;i<8;i++) {
            for (j=0;j<3;j++) {
                if (j==0) coef=new_size.x/2.0;
                if (j==1) coef=new_size.y/2.0;
                if (j==2) coef=new_size.z/2.0;
                Verts[i][j]=coef*BoxVerts[i][j];
            }
        }
    }

    void draw()
    {
        glColor3f(color.x, color.y, color.z);

        for (int i = 0 ; i < 6 ; i++) {
            const int v1 = cubeIndices[i][0];
            const int v2 = cubeIndices[i][1];
            const int v3 = cubeIndices[i][2];
            const int v4 = cubeIndices[i][3];

            glBegin (GL_QUADS) ;
            glNormal3f( bNorms[i][0],bNorms[i][1],bNorms[i][2]);
            glVertex3f( Verts[v1][0],Verts[v1][1],Verts[v1][2]);
            glNormal3f( bNorms[i][0],bNorms[i][1],bNorms[i][2]);
            glVertex3f( Verts[v2][0],Verts[v2][1],Verts[v2][2]);
            glNormal3f( bNorms[i][0],bNorms[i][1],bNorms[i][2]);
            glVertex3f( Verts[v3][0],Verts[v3][1],Verts[v3][2]);
            glNormal3f( bNorms[i][0],bNorms[i][1],bNorms[i][2]);
            glVertex3f( Verts[v4][0],Verts[v4][1],Verts[v4][2]);
            glEnd () ;
        }
    }

    bool hasIntersected(Object& obj) const override {
        auto sphere = dynamic_cast<Sphere*>(&obj);
        if (!sphere) return false;

        const double halfX = size.x * 0.5;
        const double halfY = size.y * 0.5;
        const double halfZ = size.z * 0.5;

        const double dx = sphere->center.x - center.x;
        const double dy = sphere->center.y - center.y;
        const double dz = sphere->center.z - center.z;

        const double radius = sphere->radius;

        const bool overlapX = std::abs(dx) <= (halfX + radius);
        const bool overlapY = std::abs(dy) <= (halfY + radius);
        const bool overlapZ = std::abs(dz) <= (halfZ + radius);

        return overlapX && overlapY && overlapZ;
    }

    void hitBy(Object& obj) override {
        auto sphere = dynamic_cast<Sphere*>(&obj);
        if (!sphere) return;
        if (!hasIntersected(obj)) return;

        const double halfX = size.x * 0.5;
        const double halfY = size.y * 0.5;
        const double halfZ = size.z * 0.5;

        const double dx = sphere->center.x - center.x;
        const double dy = sphere->center.y - center.y;
        const double dz = sphere->center.z - center.z;

        const double radius = sphere->radius;

        const double overlapX = (halfX + radius) - std::abs(dx);
        const double overlapY = (halfY + radius) - std::abs(dy);
        const double overlapZ = (halfZ + radius) - std::abs(dz);

        double minOverlap = overlapX;
        int axis = 0; // 0=x,1=y,2=z

        if (overlapY < minOverlap) {
            minOverlap = overlapY;
            axis = 1;
        }
        if (overlapZ < minOverlap) {
            minOverlap = overlapZ;
            axis = 2;
        }

        const double restitution = 1.0; // perfectly elastic reflection

        if (axis == 0) {
            const double direction = (dx >= 0.0) ? 1.0 : -1.0;
            obj.center.x = center.x + direction * (halfX + radius);
            obj.velocity.x = -obj.velocity.x * restitution;
            obj.accel.x = 0.0;
        } else if (axis == 1) {
            const double direction = (dy >= 0.0) ? 1.0 : -1.0;
            obj.center.y = center.y + direction * (halfY + radius);
            obj.velocity.y = -obj.velocity.y * restitution;
            obj.accel.y = 0.0;
        } else {
            const double direction = (dz >= 0.0) ? 1.0 : -1.0;
            obj.center.z = center.z + direction * (halfZ + radius);
            obj.velocity.z = -obj.velocity.z * restitution;
            obj.accel.z = 0.0;
        }
    }
};
