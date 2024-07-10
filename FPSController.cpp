#include "FPSController.h"
#include <sstream>
#include <glm/gtx/vector_angle.hpp>  // For calculating angle between vectors

FPSController::FPSController()
{
    movementSpeed = 10.0f; // Adjusted for reasonable gameplay speed
    jumpHeight = 10.0f;
    lastMoveDirection = glm::vec3(0.0f);
    directionLocked = false;
}

FPSController::~FPSController()
{

}

void FPSController::Move(SDL_Event& event, GameObject* player, Camera& camera, bool isKeyDown, float deltaTime, TerrainModel& terrainModel) {
    glm::vec3 moveDirection = glm::vec3(0.f);

    if (!isKeyDown) {
        player->model->setActiveAnimation(getAnimationString(IDLE));
        directionLocked = false; // Unlock the direction when no key is pressed
        return;
    }

    switch (event.key.keysym.sym) {
    case SDLK_w:
        moveDirection = glm::vec3(camera.Front.x, 0, -camera.Front.z);  // Ignore the y-component
        break;
    case SDLK_s:
        moveDirection = glm::vec3(-camera.Front.x, 0, camera.Front.z); // Ignore the y-component
        break;
    case SDLK_a:
        moveDirection = glm::vec3(camera.Right.x, 0, camera.Right.z); // Ignore the y-component for left movement
        break;
    case SDLK_d:
        moveDirection = glm::vec3(-camera.Right.x, 0, camera.Right.z); // Ignore the y-component for right movement
        break;
    }

    // Normalize move direction
    moveDirection = glm::normalize(moveDirection);

    if (!directionLocked) {
        lastMoveDirection = moveDirection;  // Lock the move direction
        directionLocked = true;
    }

    if (glm::length(moveDirection) > 0.0f) {
        // Calculate the angle to rotate towards the locked move direction
        glm::vec3 initialFront = glm::vec3(0, 0, 1); // Assuming the initial orientation front is along the world's Z-axis
        float angle = glm::orientedAngle(glm::vec3(initialFront.x, 0, initialFront.z), glm::vec3(lastMoveDirection.x, 0, lastMoveDirection.z), glm::vec3(0, 1, 0));

        // Apply rotation
        player->Rotation.y = glm::degrees(angle);  // Set the rotation directly to the angle calculated

        // Update player position
        player->Position += lastMoveDirection * movementSpeed * deltaTime;

        // Adjust height based on terrain
        player->Position.y = terrainModel.getHeight(player->Position.x, player->Position.z);

        // Set walking animation
        player->model->setActiveAnimation(getAnimationString(WALKING));
    }
}
