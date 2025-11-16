#include <cmath>
#include <cstring>

#include "Vec3.cpp"

// column-major: m[col*4 + row]
static inline void mulMat4_cm(double out[16], const double a[16], const double b[16]) {
    double r[16];
    for (int c=0; c<4; ++c) {
        for (int rrow=0; rrow<4; ++rrow) {
            r[c*4 + rrow] =
                a[0*4 + rrow]*b[c*4 + 0] +
                a[1*4 + rrow]*b[c*4 + 1] +
                a[2*4 + rrow]*b[c*4 + 2] +
                a[3*4 + rrow]*b[c*4 + 3];
        }
    }
    std::memcpy(out, r, sizeof(r));
}

static inline void postMultiplyTranslate(double M[16], double tx, double ty, double tz) {
    const double T[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        tx, ty, tz, 1
    };

    double out[16];
    mulMat4_cm(out, M, T);
    std::memcpy(M, out, sizeof(out));
}

// ─────────────────────────────────────────────────────────────
// Yaw: Y축 회전(도단위). 우측곱 → M = M * Ry(yaw)
static inline void postMultiplyYaw(double M[16], double yawDeg) {
    const double a = yawDeg * (M_PI / 180.0);
    const double c = std::cos(a), s = std::sin(a);

    // row-major: [[ c, 0,  s, 0],
    //             [ 0, 1,  0, 0],
    //             [-s, 0,  c, 0],
    //             [ 0, 0,  0, 1]]
    // column-major 배열:
    const double Ry[16] = {
         c, 0, -s, 0,
         0, 1,  0, 0,
         s, 0,  c, 0,
         0, 0,  0, 1
    };

    double out[16];
    mulMat4_cm(out, M, Ry);
    std::memcpy(M, out, sizeof(out));
}

// Pitch: X축 회전(도단위). 우측곱 → M = M * Rx(pitch)
static inline void postMultiplyPitch(double M[16], double pitchDeg) {
    const double a = pitchDeg * (M_PI / 180.0);
    const double c = std::cos(a), s = std::sin(a);

    // row-major: [[1, 0,  0, 0],
    //             [0,  c, -s, 0],
    //             [0,  s,  c, 0],
    //             [0,  0,  0, 1]]
    // column-major 배열:
    const double Rx[16] = {
        1, 0, 0, 0,
        0,  c, s, 0,
        0, -s, c, 0,
        0, 0, 0, 1
    };

    double out[16];
    mulMat4_cm(out, M, Rx);
    std::memcpy(M, out, sizeof(out));
}

// ─────────────────────────────────────────────────────────────
// 조합 헬퍼: 호출 순서가 "곱의 좌우"를 결정함에 주의!
static inline void postMultiplyYawThenPitch(double M[16], double yawDeg, double pitchDeg) {
    // 최종: M = M * Ry * Rx   (벡터에 적용될 때는 먼저 Rx, 그다음 Ry)
    postMultiplyYaw(M, yawDeg);
    postMultiplyPitch(M, pitchDeg);
}

static inline void postMultiplyPitchThenYaw(double M[16], double pitchDeg, double yawDeg) {
    // 최종: M = M * Rx * Ry   (벡터에 적용될 때는 먼저 Ry, 그다음 Rx)
    postMultiplyPitch(M, pitchDeg);
    postMultiplyYaw(M, yawDeg);
}

static inline void postMultiplyRotateAroundOffset(double M[16],
                                                  const Vec3& offset,
                                                  double yawDeg,
                                                  double pitchDeg) {
    const bool applyYaw = std::abs(yawDeg) > 1e-9;
    const bool applyPitch = std::abs(pitchDeg) > 1e-9;
    if (!applyYaw && !applyPitch) return;

    postMultiplyTranslate(M, -offset.x, -offset.y, -offset.z);
    if (applyYaw) postMultiplyYaw(M, yawDeg);
    if (applyPitch) postMultiplyPitch(M, pitchDeg);
    postMultiplyTranslate(M, offset.x, offset.y, offset.z);
}

// ─────────────────────────────────────────────────────────────
// 회전 기준점을 지정해 좌표를 회전시키는 도우미
static inline Vec3 rotatePointAroundPivotImpl(const Vec3& point,
                                              const Vec3& pivot,
                                              double yawDeg,
                                              double pitchDeg) {
    Vec3 relative = point - pivot;

    if (std::abs(yawDeg) > 1e-9) {
        const double yawRad = yawDeg * (M_PI / 180.0);
        const double cy = std::cos(yawRad);
        const double sy = std::sin(yawRad);
        const double x = cy * relative.x + sy * relative.z;
        const double z = -sy * relative.x + cy * relative.z;
        relative.x = x;
        relative.z = z;
    }

    if (std::abs(pitchDeg) > 1e-9) {
        const double pitchRad = pitchDeg * (M_PI / 180.0);
        const double cp = std::cos(pitchRad);
        const double sp = std::sin(pitchRad);
        const double y = cp * relative.y - sp * relative.z;
        const double z = sp * relative.y + cp * relative.z;
        relative.y = y;
        relative.z = z;
    }

    return pivot + relative;
}

static inline void rotatePointAroundPivot(Vec3& point,
                                          const Vec3& pivot,
                                          double yawDeg,
                                          double pitchDeg) {
    point = rotatePointAroundPivotImpl(point, pivot, yawDeg, pitchDeg);
}

static inline void rotateAroundPivot(double M[16],
                                     Vec3& point,
                                     const Vec3& pivot,
                                     double yawDeg,
                                     double pitchDeg) {
    const bool hasYaw = std::abs(yawDeg) > 1e-9;
    const bool hasPitch = std::abs(pitchDeg) > 1e-9;
    if (!hasYaw && !hasPitch) return;

    if (hasYaw) {
        postMultiplyYaw(M, yawDeg);
    }
    if (hasPitch) {
        postMultiplyPitch(M, pitchDeg);
    }

    rotatePointAroundPivot(point, pivot, yawDeg, pitchDeg);
}
