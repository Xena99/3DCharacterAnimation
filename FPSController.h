#pragma once

#include "SDL.h"
#include "GameObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "CameraTransformations.h"
#include "TerrainModel.h"

class FPSController
{
public:
	FPSController();
	~FPSController();

	float movementSpeed;
	float jumpHeight;
	bool directionLocked;
	glm::vec3 lastMoveDirection;

	void Move(SDL_Event& event, GameObject* gameObject, Camera& camera, bool isKeyDown, float deltaTime, TerrainModel &terrainModel);
	//void Jump();
};

