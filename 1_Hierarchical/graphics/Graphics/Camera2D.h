#pragma once

#include <glm.hpp>

class Camera2D
{
private:

	float near;
	float far;

	void buildView();
	void buildProjection();

public:
	glm::mat4 Projection;
	glm::mat4 View;

	glm::vec3 Position;
	glm::vec3 ViewDirection;
	glm::vec3 Up;

	int WinHeight, WinWidth;

	Camera2D();
	~Camera2D() {}

	virtual void Init() {}
	virtual void Update();
};

