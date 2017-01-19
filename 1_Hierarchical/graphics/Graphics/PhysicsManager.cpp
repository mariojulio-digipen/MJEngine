#include "PhysicsManager.h"
#include "GameObject.h"
#include "BodyComponent.h"
#include "Event.h"
#include "EventManager.h"

extern EventManager* eventManager;

PhysicsManager::PhysicsManager()
{
}


PhysicsManager::~PhysicsManager()
{
}


void PhysicsManager::Init()
{
  
}

void PhysicsManager::Update()
{
	// run the numerical integration
	for (auto gameObject : managedGameObjects)
	{
		if (gameObject != NULL)
		{
			if (gameObject->IsAlive)
			{
				BodyComponent* body = gameObject->FindComponent<BodyComponent>();
				body->Integrate();
			}
		}
	}

	// generate collisions and send to other game objects
	generateAllCollisions();
	sendCollisionMessage();
}

void PhysicsManager::ManageThisGameObject(GameObject* gameObject)
{
	this->managedGameObjects.push_back(gameObject);
}

void PhysicsManager::generateAllCollisions()
{
	collisionManager.Contacts.clear();

	std::list<GameObject*>::iterator it = managedGameObjects.begin();
	for (; it != managedGameObjects.end(); it++)
	{
		if (!(*it)->IsAlive)
			continue;

		std::list<GameObject*>::iterator jt = it;
		jt++;
		for (; jt != managedGameObjects.end(); jt++)
		{
			if (!(*jt)->IsAlive)
				continue;
			Contact contact;
			BodyComponent* itBody = (*it)->FindComponent<BodyComponent>();
			BodyComponent* jtBody = (*jt)->FindComponent<BodyComponent>();
			if (collisionManager.GenerateContacts(
				itBody->BodyShape, itBody->Position, 
				jtBody->BodyShape, jtBody->Position, 
				&contact))
			{
				collisionManager.Contacts.push_back(contact);
			}
		}
	}
	
}

void PhysicsManager::sendCollisionMessage()
{
	//printf("Number of contacts: %i\n", collisionManager.Contacts.size());
	std::vector<Contact>::iterator it = collisionManager.Contacts.begin();
	for (; it != collisionManager.Contacts.end(); it++)
	{
		if ((*it).contactBodies[0] != NULL)
		{
			CollisionEvent* evt = eventManager->GenerateEvent<CollisionEvent>();
			evt->C = &(*it);
			(*it).contactBodies[0]->GetOwner()->HandleEvent(evt);
			(*it).contactBodies[1]->GetOwner()->HandleEvent(evt);
			(*it).contactBodies[0] = NULL;
			delete evt;
		}
	}
}
