#pragma once

#include <algorithm>
#include <cmath>
#include <cctype>

#include "../object/Cannon.cpp"
#include "RenderManager.cpp"

class CannonManager {
public:
    explicit CannonManager(Cannon& cannonRef) {
        setCannon(cannonRef);
    }

    void setCannon(Cannon& cannonRef) {
        cannon = &cannonRef;
        syncOrbitFromCannon();
    }

    void onKeyboard(unsigned char key, bool pressed) {
        const unsigned char lower = static_cast<unsigned char>(std::tolower(key));
        switch (lower) {
            case 'a': input.left = pressed; break;
            case 'd': input.right = pressed; break;
            case 'w': input.up = pressed; break;
            case 's': input.down = pressed; break;
            default: break;
        }
    }

    void update(float deltaTime) {
        if (!cannon) return;
        integrateInputs(deltaTime);
        placeCannonOnHemisphere();
        updateCannonDirection();
        updateCamera();
    }

    const CameraState& getCamera() const {
        return camera;
    }

private:
    struct OrbitState {
        double azimuthDeg = 180.0;   // rotation around Y axis
        double elevationDeg = 30.0;  // 0 = equator, 90 = pole
    };

    struct InputState {
        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;
    };

    Cannon* cannon = nullptr;
    CameraState camera{};
    OrbitState orbit{};
    InputState input{};

    const Vec3 orbitCenter{0.0, 0.0, 0.0};
    const double orbitRadius = 35.0;
    const float cameraYOffset = 3.0f;
    const float azimuthSpeed = 60.0f;   // degrees per second
    const float elevationSpeed = 45.0f; // degrees per second
    const float minElevation = 0.0f;
    const float maxElevation = 89.0f;   // keep just below the pole to avoid gimbal lock

    void syncOrbitFromCannon() {
        if (!cannon) return;
        const Vec3 offset{
            cannon->center.x - orbitCenter.x,
            cannon->center.y - orbitCenter.y,
            cannon->center.z - orbitCenter.z
        };
        const double length = vectorLength(offset);
        if (length >= 1e-6) {
            orbit.azimuthDeg = radiansToDegrees(std::atan2(offset.x, offset.z));
            const double normalizedY = std::clamp(offset.y / orbitRadius, -1.0, 1.0);
            orbit.elevationDeg = std::clamp(radiansToDegrees(std::asin(normalizedY)),
                                            static_cast<double>(minElevation),
                                            static_cast<double>(maxElevation));
        } else {
            orbit.azimuthDeg = 0.0;
            orbit.elevationDeg = minElevation;
        }
        placeCannonOnHemisphere();
        updateCannonDirection();
        updateCamera();
    }

    void integrateInputs(float deltaTime) {
        const float yawAxis = axisValue(input.left, input.right);
        const float elevAxis = axisValue(input.up, input.down);

        orbit.azimuthDeg += yawAxis * azimuthSpeed * deltaTime;
        orbit.azimuthDeg = wrapDegrees(orbit.azimuthDeg);

        orbit.elevationDeg = std::clamp(
            orbit.elevationDeg + elevAxis * elevationSpeed * deltaTime,
            static_cast<double>(minElevation),
            static_cast<double>(maxElevation)
        );
    }

    void placeCannonOnHemisphere() {
        if (!cannon) return;
        const double yawRad = degreesToRadians(orbit.azimuthDeg);
        const double elevRad = degreesToRadians(orbit.elevationDeg);
        const double cosElev = std::cos(elevRad);
        const double sinElev = std::sin(elevRad);

        cannon->center.x = orbitCenter.x + orbitRadius * cosElev * std::sin(yawRad);
        cannon->center.z = orbitCenter.z + orbitRadius * cosElev * std::cos(yawRad);
        cannon->center.y = orbitCenter.y + orbitRadius * sinElev;

        cannon->velocity = {0.0, 0.0, 0.0};
        cannon->accel = {0.0, 0.0, 0.0};
    }

    void updateCannonDirection() {
        if (!cannon) return;
        Vec3 toCenter{
            orbitCenter.x - cannon->center.x,
            orbitCenter.y - cannon->center.y,
            orbitCenter.z - cannon->center.z
        };
        const double length = vectorLength(toCenter);
        if (length < 1e-6) {
            cannon->setDirection({0.0, 0.0, -1.0});
            return;
        }
        const double inv = 1.0 / length;
        cannon->setDirection({
            toCenter.x * inv,
            toCenter.y * inv,
            toCenter.z * inv
        });
    }

    void updateCamera() {
        if (!cannon) return;
        camera.x = static_cast<float>(cannon->center.x);
        camera.y = static_cast<float>(cannon->center.y + cameraYOffset);
        camera.z = static_cast<float>(cannon->center.z);

        const Vec3 viewDir{
            orbitCenter.x - camera.x,
            orbitCenter.y - camera.y,
            orbitCenter.z - camera.z
        };
        const double len = vectorLength(viewDir);
        if (len < 1e-6) return;
        const double inv = 1.0 / len;
        const double ny = std::clamp(viewDir.y * inv, -1.0, 1.0);

        camera.pitch = radiansToDegrees(std::asin(ny));
        camera.yaw = radiansToDegrees(std::atan2(viewDir.x, viewDir.z));
    }

    static float axisValue(bool positive, bool negative) {
        if (positive == negative) return 0.0f;
        return positive ? 1.0f : -1.0f;
    }

    static double vectorLength(const Vec3& v) {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    static double degreesToRadians(double degrees) {
        return degrees * M_PI / 180.0;
    }

    static double radiansToDegrees(double radians) {
        return radians * 180.0 / M_PI;
    }

    static double wrapDegrees(double degrees) {
        const double wrapped = std::fmod(degrees, 360.0);
        if (wrapped < 0.0) return wrapped + 360.0;
        return wrapped;
    }
};
