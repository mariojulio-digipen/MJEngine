#include "Circle.h"
#include "BodyComponent.h"
#include "TransformComponent.h"



Circle::Circle() : Radius(0.0f), Shape()
{
	Type = ShapeType::CIRCLE;
}


Circle::~Circle()
{
}

bool Circle::TestPoint(glm::vec3 point)
{
	float sqDistance = 
		(point.x - OwnerBody->Transform->GetPosition().x) * (point.x - OwnerBody->Transform->GetPosition().x) +
		(point.y - OwnerBody->Transform->GetPosition().y) * (point.y - OwnerBody->Transform->GetPosition().y);
	if (sqDistance > (Radius * Radius))
	{
		return false;
	}
	return true;
}