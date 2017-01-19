#pragma once

#include "Component.h"
#include "GameObject.h"
#include "GLObject.h"
#include "Transform.h"
#include <glm.hpp>
#include <string>
#include <sstream>


class TransformComponent : public Component
{
private:
	// Start Private properties list
	GLObject* glObject;
	GameObject* owner;
	// End Private properties list

public:
	TransformComponent() {}
	TransformComponent(GLObject* glObject) { this->glObject = glObject; }
	~TransformComponent() {}

	void SetPosition(glm::vec3& position) { glObject->Transform->Position = position; }
	void SetPosition(float x, float y, float z) 
	{ glObject->Transform->Position = glm::vec3(x, y, z); }
	void SetRotation(glm::vec3& rotation) { glObject->Transform->RotationAngle = rotation; }
	void SetRotation(float angleX, float angleY, float angleZ)
	{ glObject->Transform->RotationAngle = glm::vec3(angleX, angleY, angleZ); }
	void SetScale(glm::vec3& scale) { glObject->Transform->Size = scale; }

	glm::vec3 GetPosition() const { return glObject->Transform->Position; }
	glm::vec3 GetRotation() const { return glObject->Transform->RotationAngle; }
	glm::vec3 GetScale() const { return glObject->Transform->Size; }

	void SetQuatRotator(Quaternion q);

	// Start implementation functions
	bool Init();
	void Update();
	void Serialize(std::istringstream* objcontentStream);
	void Destroy();
	void CompleteExtraSetup();

	// End implementation functions
};

