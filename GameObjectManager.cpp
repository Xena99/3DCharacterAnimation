#include "GameObjectManager.h"

GameObjectManager::GameObjectManager()
{

}

GameObjectManager::~GameObjectManager()
{

}

void GameObjectManager::AddGameObject(string name, GameObject gameObject)
{
	gameObjects[name].push_back(gameObject);
}

void GameObjectManager::RemoveGameObject(string name)
{
	auto it = gameObjects.find(name);
	if (it != gameObjects.end() && !it->first.empty())
	{
		it->second.pop_back();
	}
}

void GameObjectManager::DrawAll(Shader &shader, float deltaTime)
{
	for (auto pair : gameObjects)
	{
		for (auto gameObject : pair.second)
		{
			gameObject.DrawGameObject(shader,deltaTime);
		}
	}
}