#pragma once

#include <map>
#include <iostream>
#include "GameObject.h"
#include <vector>

class GameObjectManager
{
public:

	GameObjectManager();
	~GameObjectManager();

	std::map<string, std::vector<GameObject>> gameObjects;
	
	void AddGameObject(string name, GameObject gameObject);
	void RemoveGameObject(string name);
	void DrawAll(Shader &shader, float deltaTime);

};

