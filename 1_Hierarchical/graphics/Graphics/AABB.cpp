#include "AABB.h"
#include "BodyComponent.h"
#include "TransformComponent.h"

AABB::AABB() : Top(0.0f), Bottom(0.0f), Right(0.0f), Left(0.0f), Shape()
{
	Type = ShapeType::RECTANGLE;
}


AABB::~AABB()
{
}

bool AABB::TestPoint(glm::vec3 point)
{
	float left, right, top, bottom;
	left = OwnerBody->Transform->GetPosition().x + Left;
	right = OwnerBody->Transform->GetPosition().x + Right;
	top = OwnerBody->Transform->GetPosition().y + Top;
	bottom = OwnerBody->Transform->GetPosition().y + Bottom;

	if ((point.x >= left) && (point.x <= right) &&
		(point.y >= top) && (point.y <= bottom)) {
		return true;
	}

	return false;
}