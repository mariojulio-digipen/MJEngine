#include "Move2DComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "InputManager.h"
#include "EventManager.h"
#include "CollisionManager.h"

extern InputManager* inputManager;
extern EventManager* eventManager;

Move2DComponent::Move2DComponent()
{
}


Move2DComponent::~Move2DComponent()
{
}


bool Move2DComponent::Init()
{
	transform = GetOwner()->FindComponent<TransformComponent>();
	if (transform)
		return true;
	return false;
}

void Move2DComponent::Update()
{
	int velo = 10;
	if (inputManager->IsKeyDown(SDL_SCANCODE_UP))
	{
		transform->SetPosition(
			glm::vec3(transform->GetPosition().x, transform->GetPosition().y + velo, 0));
	}
	if (inputManager->IsKeyDown(SDL_SCANCODE_DOWN))
	{
		transform->SetPosition(
			glm::vec3(transform->GetPosition().x, transform->GetPosition().y - velo, 0));
	}
	if (inputManager->IsKeyDown(SDL_SCANCODE_LEFT))
	{
		transform->SetPosition(
			glm::vec3(transform->GetPosition().x - velo, transform->GetPosition().y, 0));
	}
	if (inputManager->IsKeyDown(SDL_SCANCODE_RIGHT))
	{
		transform->SetPosition(
			glm::vec3(transform->GetPosition().x + velo, transform->GetPosition().y, 0));
	}
}

void Move2DComponent::Destroy()
{

}

void Move2DComponent::HandleEvent(const Event * const evt)
{
	const CollisionEvent* collisionEvent = dynamic_cast<const CollisionEvent*>(evt);
	if (!collisionEvent) return;

	GameObject* object1 = collisionEvent->C->contactBodies[0]->GetOwner();
	GameObject* object2 = collisionEvent->C->contactBodies[1]->GetOwner();
	if (object1 != GetOwner())
	{
		StartOrbit* evt = eventManager->GenerateEvent<StartOrbit>();
		evt->AngularVelocity = 5.0f;
		eventManager->BroadCastEvent(evt);
	}
}