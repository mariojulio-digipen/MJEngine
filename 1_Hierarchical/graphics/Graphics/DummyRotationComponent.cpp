#include "DummyRotationComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "Shader.h"
#include "FrameRateController.h"
#include "EventManager.h"
#include "CollisionManager.h"
#include "BodyComponent.h"

extern FrameRateController* frameRateController;

DummyRotationComponent::DummyRotationComponent() :
	angle(0.0f),
	angularVel(1.0f),
	direction(0),
	radius(0)
{
}


DummyRotationComponent::~DummyRotationComponent()
{
}


bool DummyRotationComponent::Init()
{
	transform = GetOwner()->FindComponent<TransformComponent>();
	if (!transform)
		return false;
	return true;
}

void DummyRotationComponent::Update()
{
	if (GetOwner()->GetGLObject()->Transform->TransType == TRANSFORM_TYPE::WORLD)
	{
		if (GetOwner()->GetName() == "UILoading")
		{
			angle = (angle >= 360.0f) ? 0.0f : angle + frameRateController->DeltaTime;
			transform->SetRotation(glm::vec3(0.0f, 0.0f, angle));
		}
		else
		{
			angle = (angle >= 360.0f) ? 0.0f : angle + direction * frameRateController->DeltaTime * angularVel;
			transform->SetPosition(glm::vec3(radius * cosf(glm::radians(angle)), transform->GetPosition().y, radius * sinf(glm::radians(angle))));
			transform->SetRotation(glm::vec3(transform->GetRotation().x, -angle + 180.0f, transform->GetRotation().z));
		}
	}
	else if (GetOwner()->GetGLObject()->Transform->TransType == TRANSFORM_TYPE::LOCAL)
	{
		angle = (angle >= 360.0f) ? 0.0f : angle + frameRateController->DeltaTime;
		transform->SetRotation(glm::vec3(0.0f, 0.0f, angle));
	}
	/*angle = (angle >= 360.0f) ? 0.0f : angle + frameRateController->DeltaTime;
	if (GetOwner()->GetName() == "UILoading")
		transform->SetRotation(glm::vec3(0.0f, 0.0f, angle));
	else if (GetOwner()->GetName() == "Sphere")
	{
		angle = (angle >= 360.0f) ? 
			0.0f : angle + direction * frameRateController->DeltaTime * angularVel*0.1;
		transform->SetPosition(
			glm::vec3(radius * cosf(glm::radians(angle)),
				transform->GetPosition().y, radius * sinf(glm::radians(angle))));
		transform->SetRotation(
			glm::vec3(transform->GetRotation().x, 
				-angle + 180.0f, transform->GetRotation().z));
	}
	else
		transform->SetRotation(glm::vec3(
			transform->GetRotation().x, 
			angle, 
			transform->GetRotation().z));*/

	
	
}

void DummyRotationComponent::Destroy()
{

}

void DummyRotationComponent::HandleEvent(const Event* const evt)
{

	const StartOrbit* orbitEvent = dynamic_cast<const StartOrbit*>(evt);
	if (orbitEvent != NULL)
	{
		if (GetOwner()->GetName() == "pokeball")
			angularVel = orbitEvent->AngularVelocity;
	}

	

	/*const CollisionEvent* colEvent = dynamic_cast<const CollisionEvent*>(evt);
	if (colEvent != NULL)
	{
		printf("this is a collision event");
	}

	const DummyEvent* dummye = dynamic_cast<const DummyEvent*>(evt);
	if (dummye != NULL)
	{
		printf("this is a DummyEvent event");
	}*/

	
}

void DummyRotationComponent::Serialize(std::istringstream* objcontentStream)
{
	std::string line;
	std::getline((*objcontentStream), line);

	std::istringstream lineStream(line.c_str());
	std::string word;

	while ((*objcontentStream) >> word)
	{
		if (word == "#")
		{
			std::getline((*objcontentStream), line);
			continue;
		}
		if (word == "d")
		{
			std::getline((*objcontentStream), line);
			direction = stoi(line);
			continue;
		}
		if (word == "r")
		{
			std::getline((*objcontentStream), line);
			radius = stoi(line);
			continue;
		}
		if (word == "end_DummyRotationComponent")
		{
			break;
		}

	}
}