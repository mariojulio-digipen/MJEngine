#pragma once

#include "BodyComponent.h"
#include "Shape.h"

#include <glm.hpp>
#include <vector>

class Contact
{
public:
	Contact();
	~Contact() {}
	BodyComponent* contactBodies[2];
	glm::vec3 normal;
};

typedef bool(*CollisionTest)(
	Shape* shape1, glm::vec3 posShape1, 
	Shape* shape2, glm::vec3 posShape2, 
	Contact* contacts);

class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

	std::vector<Contact> Contacts;
	bool GenerateContacts(
		Shape* a, glm::vec3 aPos, 
		Shape* b, glm::vec3 bPos, 
		Contact* contacts);

	//void ResetContacts();
	CollisionTest 
		CollsionTestTable[Shape::ShapeType::MAX_SHAPE][Shape::ShapeType::MAX_SHAPE];
};

