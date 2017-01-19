#pragma once

#include "Component.h"
#include <glm.hpp>

class TransformComponent;
class DummyRotationComponent : public Component
{
private:
	TransformComponent* transform;
	float angle;
	float angularVel;
	int direction;
	int radius;
public:
	DummyRotationComponent();
	~DummyRotationComponent();

	// Autogenerated by MJEngine automation tools
	// Start implementation functions
	bool Init();
	void Update();
	void Serialize(std::istringstream* objcontentStream);
	void HandleEvent(const Event* const evt);
	void Destroy();
	// End implementation functions
};
