#include "GameObject.h"

void GameObject::DrawGameObject(Shader& shader, float alpha)
{
    if (model)
    {
        glm::mat4 modelMatrix = ComputeModelMatrix(Position, Rotation, Scale);
        shader.setMat4("model", modelMatrix);
        model->Draw(shader, alpha);
    }
}

glm::vec3 GameObject::GetForwardVector()
{
    glm::vec3 forwardVector;
    forwardVector.x = cos(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x));
    forwardVector.y = sin(glm::radians(Rotation.x));
    forwardVector.z = sin(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x));
    return glm::normalize(forwardVector);
}

glm::vec3 GameObject::GetRightVector()
{
    glm::vec3 forward = GetForwardVector();
    glm::vec3 worldUp = glm::vec3(0.f, 1.f, 0.f);
    return glm::normalize(glm::cross(worldUp, forward));
}

glm::vec3 GameObject::GetUpVector()
{
    glm::vec3 forward = GetForwardVector();
    glm::vec3 right = GetRightVector();
    return glm::normalize(glm::cross(forward, right));
}

glm::mat4 GameObject::ComputeModelMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.f, 0.f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.f, 0.f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.f, 1.f));
    modelMatrix = glm::scale(modelMatrix, scale);
    return modelMatrix;
}