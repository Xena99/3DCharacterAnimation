#include "CameraControls.h"
#include <algorithm>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

template <typename T>
T clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

CameraControls::CameraControls() {
    mouseSensitivity = 50.f;
}

CameraControls::~CameraControls() {}

void CameraControls::ProcessMouseInputs(Camera* camera, float x, float y, float deltaTime) {
    x *= mouseSensitivity * deltaTime;
    y *= mouseSensitivity * deltaTime;

    // Calculate quaternion for yaw rotation (around the world up vector)
    glm::quat qYaw = glm::angleAxis(glm::radians(x), camera->WorldUp);

    // Calculate quaternion for pitch rotation (around the camera's local right vector)
    glm::quat qPitch = glm::angleAxis(glm::radians(y), camera->Right);

    // Update the camera orientation by combining yaw and pitch rotations
    camera->Orientation = camera->Orientation * qYaw * qPitch;

    // Normalize the orientation quaternion to avoid precision issues
    camera->Orientation = glm::normalize(camera->Orientation);

    // Update camera vectors based on the new orientation
    camera->updateCameraVectors();

  
}

void CameraControls::UpdateCameraPosition(Camera* camera, GameObject& player) {
    // Attach the camera position directly to the player position
    camera->Position = player.Position;
}