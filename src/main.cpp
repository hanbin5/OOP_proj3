#include <iostream>

/* Simple geometry viewer:  Left mouse: rotate;  Right mouse:   translate;  ESC to quit. */
#include <assert.h>
#include <math.h>
#include <cmath>
#include <stdlib.h>

#include <GLUT/glut.h>

//GLdouble rotMatrix[4][16];
const int NO_SPHERE=3;
const int WALL_ID=1000;
const int Floor_ID=2000;

int rotate_x=0, rotate_y=0;
int choice=1;

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

/* Viewer state */
float sdepth = 10;
float zNear=1.0, zFar=100.0;
float aspect = 5.0/4.0;
float xcam = 0, ycam = 0;
long xsize, ysize;
int downX, downY;
bool leftButton = false, middleButton = false, rightButton = false;
int i,j;
GLfloat light0Position[] = { 0, 1, 0, 1.0};
int displayMenu, mainMenu;

void MyIdleFunc(void) { glutPostRedisplay();} /* things to do while idle */
void RunIdleFunc(void) {   glutIdleFunc(MyIdleFunc); }
void PauseIdleFunc(void) {   glutIdleFunc(NULL); }
void renderScene();

class CObject
{
public:
    Vec3 center;
    Vec3 color;
    Vec3 velocity;
    Vec3 accel;
    double mass;

    virtual ~CObject() = default;

    void setColor(double r, double g, double b) {
        color = {r, g, b};
    }

    void setCenter(Vec3& new_loc) {
        center.x = new_loc.x; center.y = new_loc.y; center.z = new_loc.z;
    }
    void setCenter(double x, double y, double z) {
        center.x = x; center.y = y; center.z = z;
    }

    void move() {
        velocity = velocity + accel;
        Vec3 new_loc = center + velocity;
        setCenter(new_loc);
    }

    virtual bool hasIntersected(CObject& obj) const = 0;

    void hitBy(CObject& obj) {
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

        std::cout << velocity.x << velocity.y << velocity.z << std::endl;
    }
};

class CSphere : public CObject 
{
public:
    double radius;

    GLdouble m_mRotate[16];
    CSphere(double radius = 0.5) {
        this->radius = radius;
        center = {0, 0, 0};
        color = {0, 0, 0};
        velocity = {0, 0, 0};
        accel = {0, 0, 0};

        mass = std::pow(radius, 3) * (4.0/3.0) * 3.14;
    }

    void init() {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glGetDoublev(GL_MODELVIEW_MATRIX, m_mRotate);
        glPopMatrix();
    }

    void draw()
    {
        glLoadIdentity();
        glTranslatef(0.0,0.0,-sdepth);
        glMultMatrixd(m_mRotate);
        glTranslated(center.x,center.y,center.z);
        glColor3f(color.x, color.y, color.z);
        glutSolidSphere(radius, 20, 20);
    }

    bool hasIntersected(CObject& obj) const override {
        auto ball = dynamic_cast<const CSphere*>(&obj);
        if (!ball) return false;

        double dx = center.x - ball->center.x;
        double dy = center.y - ball->center.y;
        double dz = center.z - ball->center.z;
        double rsum = radius + ball->radius;
        return (dx*dx + dy*dy + dz*dz) <= (rsum * rsum);
    }
};

class CFloor : public CObject
{
public:
    GLdouble m_mRotate[16];
    float width, height, depth;
    GLfloat Verts[8][3];

    CFloor(float w=500, float h=1, float d=500) {
        width=w; height=h; depth=d;
        setColor(0.0, 0.5, 1.0);

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

    void init()
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glGetDoublev(GL_MODELVIEW_MATRIX, m_mRotate);
        glPopMatrix();
    }

    void draw()
    {
        glLoadIdentity();
        glTranslatef(0.0,0.0,-sdepth);
        glMultMatrixd(m_mRotate);
        glTranslatef(center.x,center.y,center.z);

        glColor3f(color.x, color.y, color.z);

        int i;
        int v1,v2,v3,v4;

        for (i = 0 ; i < 6 ; i++) {
            v1 = cubeIndices[i][0];
            v2 = cubeIndices[i][1];
            v3 = cubeIndices[i][2];
            v4 = cubeIndices[i][3];

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

    bool hasIntersected(CObject& obj) const override {
        return false;
    }
};

class CWall : public CObject
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

    void init()
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glGetDoublev(GL_MODELVIEW_MATRIX, m_mRotate);
        glPopMatrix();
    }

    void setSize(float w, float h, float d)
    {
        width=w;
        height=h;
        depth=d;
    }

    void draw()
    {
        glLoadIdentity();
        glTranslatef(0.0,0.0,-sdepth);
        glMultMatrixd(m_mRotate);
        glTranslatef(center.x,center.y,center.z);

        glColor3f(color.x, color.y, color.z);

        int i;
        int v1,v2,v3,v4;

        for (i = 0 ; i < 6 ; i++) {
            v1 = cubeIndices[i][0];
            v2 = cubeIndices[i][1];
            v3 = cubeIndices[i][2];
            v4 = cubeIndices[i][3];

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

    bool hasIntersected(CObject& obj) const override {
        return false;
    }
};

CSphere g_sphere[3];
CFloor g_floor;

void ReshapeCallback(int width, int height)
{
    xsize = width;
    ysize = height;
    aspect = (float)xsize/(float)ysize;
    glViewport(0, 0, xsize, ysize);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(64.0, aspect, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutPostRedisplay();
}

void DisplayCallback(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    for (i=0;i<NO_SPHERE;i++) {
        for (int j = i; j < NO_SPHERE; j++) {
            g_sphere[i].hitBy(g_sphere[j]);
        }
    }

    for (i=0;i<NO_SPHERE;i++) g_sphere[i].draw();
    g_floor.draw();

    glutSwapBuffers();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int space_flag=0;

void KeyboardCallback(unsigned char ch, int x, int y)
{
    switch (ch)
    {
    case '1' : choice=1; break;
    case '2' : choice=2; break;
    case '3' : choice=3; break;

    case 32 :
        if (space_flag) space_flag=0;
        else {
            space_flag=1;
            g_sphere[0].velocity.x = g_sphere[2].center.x - g_sphere[0].center.x;
            g_sphere[0].velocity.y = g_sphere[2].center.y - g_sphere[0].center.y;
            g_sphere[0].velocity.z = g_sphere[2].center.z - g_sphere[0].center.z;
        }
        break; // SPACE_KEY

    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void MouseCallback(int button, int state, int x, int y)
{
    downX = x; downY = y;
    leftButton = ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN));
    middleButton = ((button == GLUT_MIDDLE_BUTTON) &&  (state == GLUT_DOWN));
    rightButton = ((button == GLUT_RIGHT_BUTTON) &&  (state == GLUT_DOWN));
    glutPostRedisplay();
}

void rotate(int id)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glRotated(((double)rotate_y), 1.0, 0.0, 0.0);
    glRotated(((double)rotate_x), 0.0, 1.0, 0.0);

    if (id<NO_SPHERE) {
        glGetDoublev(GL_MODELVIEW_MATRIX, g_sphere[id].m_mRotate);
    }

    if (id==Floor_ID) {
        glGetDoublev(GL_MODELVIEW_MATRIX, g_floor.m_mRotate);
    }
    glPopMatrix();
}

void MotionCallback(int x, int y) {
    int tdx=x-downX,tdy=0,tdz=y-downY,id=choice-1;
    if (leftButton) {
        rotate_x += x-downX;
        rotate_y += y-downY;
        for (i=0;i<NO_SPHERE;i++) rotate(i);
        rotate(Floor_ID);
    } else if (rightButton) {
        if (id<NO_SPHERE) g_sphere[id].setCenter(g_sphere[id].center.x+tdx/100.0,g_sphere[id].center.y+tdy/100.0,g_sphere[id].center.z+tdz/100.0);
    }
    downX = x;   downY = y;
    glutPostRedisplay();
}

void initRotate() {
    g_sphere[0].init();
    g_sphere[1].init();
    g_sphere[2].init();
    g_floor.init();
}

void InitGL() {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1920, 1080);
    glutCreateWindow("OpenGL Applications");
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.7, 0.7, 0.7, 0.0);
    glPolygonOffset(1.0, 1.0);
    glDisable(GL_CULL_FACE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glLightfv (GL_LIGHT0, GL_POSITION, light0Position);
    glEnable(GL_LIGHT0);
    initRotate();

    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);

    glutIdleFunc(renderScene);
    glutReshapeFunc(ReshapeCallback);
    glutDisplayFunc(DisplayCallback);
    glutKeyboardFunc(KeyboardCallback);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(MotionCallback);
}

int currentTime, previousTime=-1;
void renderScene()
{
    int timeDelta;
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (previousTime==-1) timeDelta=0;
    else timeDelta = currentTime - previousTime;
    for (int i = 0; i < NO_SPHERE; i++) {
        float x=g_sphere[i].center.x;
        float y=g_sphere[i].center.y;
        float z=g_sphere[i].center.z;

        if (space_flag) g_sphere[i].setCenter(
            x+timeDelta*0.002*g_sphere[i].velocity.x,
            y+timeDelta*0.002*g_sphere[i].velocity.y,
            z+timeDelta*0.002*g_sphere[i].velocity.z);
    }
    glutPostRedisplay();
    previousTime=currentTime;

}

void InitObjects()
{
    // specify initial colors and center positions of each spheres
    g_sphere[0].setColor(0.8,0.2,0.2); g_sphere[0].setCenter(2.0,0.5,2.0);
    g_sphere[1].setColor(0.2,0.8,0.2); g_sphere[1].setCenter(2.0,0.5,-2.0);
    g_sphere[2].setColor(0.2,0.2,0.8); g_sphere[2].setCenter(0.0,0.5,0.0);

    g_floor.setColor(0.0, 0.4, 0.8); g_floor.setCenter(0.0, -0.5, 0.0);

    // specify initial colors and center positions of a wall
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    InitObjects();
    InitGL();
    glutMainLoop();
    return 0;
}
