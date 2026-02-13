#include "Camera.h"
#include <cmath>

namespace {
    constexpr float MAX_PITCH_RADIANS = 1.5f;
}

Camera::Camera() : position(0, 0, 0), yaw(0.0f), pitch(0.0f) {
    updateVectors();
}

Camera::Camera(const Vector3& pos, float yaw, float pitch)
    : position(pos), yaw(yaw), pitch(pitch) {
    updateVectors();
}

void Camera::updateVectors() {
    // Calculate forward vector
    forward.x = std::cos(pitch) * std::cos(yaw);
    forward.y = std::sin(pitch);
    forward.z = std::cos(pitch) * std::sin(yaw);
    forward = forward.normalize();

    // Calculate right vector
    Vector3 worldUp(0, 1, 0);
    right.x = forward.z;
    right.y = 0;
    right.z = -forward.x;
    right = right.normalize();

    // Calculate up vector
    up.x = right.y * forward.z - right.z * forward.y;
    up.y = right.z * forward.x - right.x * forward.z;
    up.z = right.x * forward.y - right.y * forward.x;
}

void Camera::setPosition(const Vector3& pos) {
    position = pos;
}

void Camera::rotate(float deltaYaw, float deltaPitch) {
    yaw += deltaYaw;
    pitch += deltaPitch;

    // Clamp pitch to prevent camera flipping
    if (pitch > MAX_PITCH_RADIANS) pitch = MAX_PITCH_RADIANS;
    if (pitch < -MAX_PITCH_RADIANS) pitch = -MAX_PITCH_RADIANS;

    updateVectors();
}

void Camera::moveForward(float distance) {
    position = position + forward * distance;
}

void Camera::moveRight(float distance) {
    position = position + right * distance;
}

Vector3 Camera::getPosition() const {
    return position;
}

Vector3 Camera::getForward() const {
    return forward;
}

Vector3 Camera::getRight() const {
    return right;
}

Vector3 Camera::getUp() const {
    return up;
}

float Camera::getYaw() const {
    return yaw;
}

float Camera::getPitch() const {
    return pitch;
}
