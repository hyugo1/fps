#ifndef CAMERA_H
#define CAMERA_H

#include "Vector3.h"

class Camera {
private:
    Vector3 position;
    float yaw;   // Horizontal rotation
    float pitch; // Vertical rotation
    Vector3 forward;
    Vector3 right;
    Vector3 up;

    void updateVectors();

public:
    Camera();
    Camera(const Vector3& pos, float yaw = 0.0f, float pitch = 0.0f);

    void setPosition(const Vector3& pos);
    void rotate(float deltaYaw, float deltaPitch);
    void moveForward(float distance);
    void moveRight(float distance);

    Vector3 getPosition() const;
    Vector3 getForward() const;
    Vector3 getRight() const;
    Vector3 getUp() const;
    float getYaw() const;
    float getPitch() const;
};

#endif // CAMERA_H
