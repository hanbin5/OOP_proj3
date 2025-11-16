#pragma once

#include <GLUT/glut.h>

#include "ObjectManager.cpp"
#include "CameraManager.cpp"

struct RenderCallbacks {
    void (*display)() = nullptr;
    void (*reshape)(int, int) = nullptr;
    void (*keyboard)(unsigned char, int, int) = nullptr;
    void (*keyboardUp)(unsigned char, int, int) = nullptr;
    void (*mouse)(int, int, int, int) = nullptr;
    void (*motion)(int, int) = nullptr;
    void (*idle)() = nullptr;
    void (*special)(int, int, int) = nullptr;
    void (*specialUp)(int, int, int) = nullptr;
};

class RenderManager {
public:
    void applyDefaultGLState() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.7f, 0.7f, 0.7f, 0.0f);
        glDisable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        const GLfloat lightPos[] = {0.0f, 10.0f, 0.0f, 1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    }

    void resize(int width, int height) {
        if (height <= 0) height = 1;
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        gluPerspective(60.0f, aspect, 0.1f, 1000.0f);
        glMatrixMode(GL_MODELVIEW);
    }

    void registerCallbacks(const RenderCallbacks& callbacks) {
        if (callbacks.display) glutDisplayFunc(callbacks.display);
        if (callbacks.reshape) glutReshapeFunc(callbacks.reshape);
        if (callbacks.keyboard) glutKeyboardFunc(callbacks.keyboard);
        if (callbacks.keyboardUp) glutKeyboardUpFunc(callbacks.keyboardUp);
        if (callbacks.mouse) glutMouseFunc(callbacks.mouse);
        if (callbacks.motion) glutMotionFunc(callbacks.motion);
        if (callbacks.idle) glutIdleFunc(callbacks.idle);
        if (callbacks.special) glutSpecialFunc(callbacks.special);
        if (callbacks.specialUp) glutSpecialUpFunc(callbacks.specialUp);
    }

    void drawAll(ObjectManager& objectManager, CameraManager& cameraManager) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glRotatef(-cameraManager.getPitch(), 1.0f, 0.0f, 0.0f);
        glRotatef(-cameraManager.getYaw(), 0.0f, 1.0f, 0.0f);
        glTranslatef(-cameraManager.getX(), -cameraManager.getY(), -cameraManager.getZ());

        for (const auto& objPtr : objectManager.getObjectList()) {
            glPushMatrix();
            glTranslated(objPtr->center.x, objPtr->center.y, objPtr->center.z);
            glMultMatrixd(objPtr->m_mRotate);
            objPtr->draw();
            glPopMatrix();
        }

        //glTranslatef(cameraManager.getX(), cameraManager.getY(), cameraManager.getZ());
        //glRotatef(cameraManager.getYaw(), 0.0f, 1.0f, 0.0f);
        //glRotatef(cameraManager.getPitch(), 1.0f, 0.0f, 0.0f);
        
        glPopMatrix();
    }
};
