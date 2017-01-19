// Autogenerated by MJEngine automation tools
// Edition author: mzaldivar

#pragma once

#include "Component.h"

#include <sstream>
#include <glm.hpp>
#include <glew.h>

class TransformComponent;
class Shape;
class GameObject;

class BodyComponent : public Component
{
private:

	// Start Private properties list
	GameObject* debugquad;
	// End Private properties list

	void updateTransform();
	

public:

	// Start Public properties list
	TransformComponent* Transform;
	Shape* BodyShape;

	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Velocity;
	glm::vec3 Accelaration;
	glm::vec3 Force;
	glm::vec3 AngularVelocity;
	float Mass, InverseMass;
	// End Public properties list


	BodyComponent();
	~BodyComponent();

	// Start implementation functions
	bool Init();
	void Update();
	void Serialize(std::istringstream* objcontentStream);
	void Destroy();
	void CompleteExtraSetup();
	// End implementation functions

	// Start user defined functions
	void Integrate();
	// End user defined functions
};

