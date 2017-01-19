#pragma once



#include <glm.hpp>


class BodyComponent;
class Shape
{
public:

	enum ShapeType
	{
		CIRCLE,
		RECTANGLE,
		MAX_SHAPE
	};

	Shape();
	virtual ~Shape();

	ShapeType Type;
	BodyComponent* OwnerBody;
	virtual bool TestPoint(glm::vec3 point) = 0;
};

