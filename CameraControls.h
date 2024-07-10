#pragma once

#include "MovementEnum.h"
#include "CameraTransformations.h"
#include "GameObject.h"

class CameraControls
{
public: 
	CameraControls();
	~CameraControls();

	float mouseSensitivity;

	void ProcessMouseInputs(Camera* camera, float x, float y, float deltaTime);
	void UpdateCameraPosition(Camera* camera, GameObject &player);
	

};

