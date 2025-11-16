#pragma once

#include <string>

#include <GLUT/glut.h>

#include "RenderManager.cpp"

struct WindowConfig {
    int width = 1920;
    int height = 1080;
    std::string title = "Akanoid";
    unsigned int displayMode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;
};

class WindowManager {
public:
    void initialize(int* argc, char** argv, const WindowConfig& config = WindowConfig{}) {
        glutInit(argc, argv);
        glutInitDisplayMode(config.displayMode);
        glutInitWindowSize(config.width, config.height);
        glutCreateWindow(config.title.c_str());
        renderManager.applyDefaultGLState();
    }

    RenderManager& getRenderManager() {
        return renderManager;
    }

    void registerCallbacks(const RenderCallbacks& callbacks) {
        renderManager.registerCallbacks(callbacks);
    }

    void enterMainLoop() {
        glutMainLoop();
    }

private:
    RenderManager renderManager;
};
