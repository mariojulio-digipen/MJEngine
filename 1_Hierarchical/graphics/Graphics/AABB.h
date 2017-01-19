#pragma once

#include "Shape.h"

class AABB : public Shape
{
public:
	AABB();
	~AABB();

	float Left, Right, Top, Bottom;
	bool TestPoint(glm::vec3 point);
};

