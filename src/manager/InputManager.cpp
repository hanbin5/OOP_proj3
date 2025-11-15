#pragma once

#include <functional>

class InputManager {
public:
    void setKeyboardCallback(const std::function<void(unsigned char, int, int)>& callback) {
        keyboardCallback = callback;
    }

    void setMouseCallback(const std::function<void(int, int, int, int)>& callback) {
        mouseCallback = callback;
    }

    void setMotionCallback(const std::function<void(int, int)>& callback) {
        motionCallback = callback;
    }

    void onKeyboard(unsigned char key, int x, int y) {
        if (keyboardCallback) keyboardCallback(key, x, y);
    }

    void onMouse(int button, int state, int x, int y) {
        if (mouseCallback) mouseCallback(button, state, x, y);
    }

    void onMotion(int x, int y) {
        if (motionCallback) motionCallback(x, y);
    }

private:
    std::function<void(unsigned char, int, int)> keyboardCallback;
    std::function<void(int, int, int, int)> mouseCallback;
    std::function<void(int, int)> motionCallback;
};
