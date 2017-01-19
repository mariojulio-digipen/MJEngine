#pragma once

#include "Shape.h"

class Circle : public Shape
{
public:
	Circle();
	~Circle();

	float Radius;
	bool TestPoint(glm::vec3 point);
};

