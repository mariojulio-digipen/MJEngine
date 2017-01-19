#pragma once

#include "CollisionManager.h"
#include <list>

class GameObject;
class PhysicsManager
{
private:
	CollisionManager collisionManager;
	std::list<GameObject*> managedGameObjects;
	void generateAllCollisions();
	void sendCollisionMessage();

public:

	const float GRAVITY = -0.15f;

	PhysicsManager();
	~PhysicsManager();

	void Init();
	void Update();
	void ManageThisGameObject(GameObject* gameObject);


};

