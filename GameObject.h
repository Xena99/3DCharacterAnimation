#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Model.h"

class GameObject
{
public:
    string name;
    glm::vec3 Position;
    glm::vec3 Scale;
    glm::vec3 Rotation;
    Model* model;

    GameObject(std::string name, glm::vec3 pos, glm::vec3 scale, glm::vec3 rotation, Model* model) :
       name(name), Position(pos), Rotation(rotation), Scale(scale), model(model)
    {

    }

    ~GameObject()
    {

    }

    void DrawGameObject(Shader &shader, float alpha);
    glm::vec3 GetForwardVector();
    glm::vec3 GetRightVector();
    glm::vec3 GetUpVector();
    glm::mat4 ComputeModelMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
};
