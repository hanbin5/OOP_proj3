#pragma once

#include <string>

#include <GLUT/glut.h>

#include "ObjectManager.cpp"
#include "PhysicsManager.cpp"
#include "FileManager.cpp"
#include "TimeManager.cpp"
#include "WindowManager.cpp"
#include "StageManager.cpp"

class GameManager {
public:
    GameManager() : stageFile("stage1.txt") {}

    void setStageFile(const std::string& path) {
        stageFile = path;
    }

    void configureWindow(const WindowConfig& config) {
        windowConfig = config;
    }

    void play(int& argc, char** argv) {
        instance = this;

        windowManager.initialize(&argc, argv, windowConfig);

        RenderCallbacks callbacks{};
        callbacks.display = &GameManager::DisplayProxy;
        callbacks.idle = &GameManager::IdleProxy;
        callbacks.reshape = &GameManager::ReshapeProxy;
        callbacks.keyboard = &GameManager::KeyboardProxy;
        callbacks.keyboardUp = &GameManager::KeyboardUpProxy;
        callbacks.special = &GameManager::SpecialProxy;
        callbacks.specialUp = &GameManager::SpecialUpProxy;
        windowManager.registerCallbacks(callbacks);

        stageManager.onResize(windowConfig.width, windowConfig.height);

        windowManager.enterMainLoop();
    }

private:
    static GameManager* instance;

    FileManager fileManager;
    WindowManager windowManager;
    StageManager stageManager{fileManager, windowManager.getRenderManager()};
    WindowConfig windowConfig;
    bool stageLoaded = false;
    std::string stageFile;

    void renderFrame() {
        if (!stageLoaded) {
            stageManager.loadStage(stageFile);
            stageLoaded = true;
        }
        stageManager.render();
    }

    void updateFrame() {
        stageManager.update();
        glutPostRedisplay();
    }

    static void DisplayProxy() {
        if (instance) instance->renderFrame();
    }

    static void IdleProxy() {
        if (instance) instance->updateFrame();
    }

    static void ReshapeProxy(int width, int height) {
        if (instance) instance->stageManager.onResize(width, height);
    }

    static void KeyboardProxy(unsigned char key, int x, int y) {
        if (instance) instance->stageManager.onKeyboard(key, x, y, true);
    }

    static void KeyboardUpProxy(unsigned char key, int x, int y) {
        if (instance) instance->stageManager.onKeyboard(key, x, y, false);
    }

    static void SpecialProxy(int key, int x, int y) {
        if (instance) instance->stageManager.onSpecial(key, x, y, true);
    }

    static void SpecialUpProxy(int key, int x, int y) {
        if (instance) instance->stageManager.onSpecial(key, x, y, false);
    }

};

GameManager* GameManager::instance = nullptr;
