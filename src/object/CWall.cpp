#pragma once

#include "Object.cpp"

class CWall : public Object
{

public:
    float width, height, depth;

    GLfloat Verts[8][3];

public :
    GLdouble m_mRotate[16];
    CWall(float w=0.0, float h=0.0, float d=0.0)
    {
        width=w; height=h; depth=d;
        setColor(0.0, 1.0, 0.0);

        int i,j;
        float coef;
        for (i=0;i<8;i++) {
            for (j=0;j<3;j++) {
                if (j==0) coef=w/2.0;
                if (j==1) coef=h/2.0;
                if (j==2) coef=d/2.0;
                Verts[i][j]=coef*BoxVerts[i][j];
            }
        }
    }

    void setSize(float w, float h, float d)
    {
        width=w;
        height=h;
        depth=d;
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
        return false;
    }
};
