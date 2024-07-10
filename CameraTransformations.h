#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    glm::quat Orientation;
    float CurrentPitch = 0.0f;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), WorldUp(up), Orientation(glm::quat()), Right(glm::vec3(1.0, 0.0, 0.f))
    {
        Position = position;
        updateCameraVectors();
    }

    // returns the view matrix calculated using quaternion-based LookAt
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position - Front, Up);
    }

    void updateCameraVectors() {
       
        Front = glm::rotate(Orientation, glm::vec3(0.0f, 0.0f, -1.0f));
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
