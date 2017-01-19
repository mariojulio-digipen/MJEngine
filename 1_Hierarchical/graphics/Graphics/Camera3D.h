#pragma once

#include <glm.hpp>
#include "MouseManager.h"
#include "InputManager.h"


class Ray
{
private:
	glm::vec2 viewportCoords; // with mouse position in the screen
	glm::vec3 ndcCoords; // with camera position in the world

	glm::mat4 projection;
	glm::mat4 view;
	glm::vec3 direction;

public:
	Ray();
	Ray(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) :
		projection(projectionMatrix), view(viewMatrix) {}
	~Ray();

	void Cast();
	glm::vec3 GetDirection() { return direction; }

};

class Camera3D
{

private:
	bool isRayCaster = true;
	bool rayCasted = false;
	bool stopCasting = false;

	float near;
	float far;
	glm::vec2 translateDelta;

	glm::vec2 orbitStartPos;
	bool orbitStart;

	void buildView();
	void orbit(glm::vec3 target);
	void buildProjection();
	void move();
	void keyboardStateUpdate();
	void mouseEventsUpdate();

public:

	glm::vec2 moveDelta;
	glm::vec2 angle;

	glm::mat4 Projection;
	glm::mat4 View;

	glm::vec3 Position;
	glm::vec3 ViewDirection;
	glm::vec3 OldViewDirection;
	glm::vec3 Up; 
	glm::vec3 Right;

	glm::mat4 ModelToWorld;
	glm::mat4 ModelTransform;

	glm::vec3 CamRayDirection;

	int Width;
	int Height;
	float Ratio;
	float FovAngleDegrees;

	MouseManager* MouseMove;
	InputManager* InputMove;

	Camera3D() : near(0.1f), far(1000.0f), translateDelta(glm::vec2(1.0f, 1.0f)), angle(glm::vec2(0.0f, 0.0f)) {}
	~Camera3D() {}

	virtual void Init() {}
	virtual void Update();

	bool IsRayCaster() { return isRayCaster;}
	bool CastedRay() { return rayCasted; }
	bool StopCasting() { return stopCasting; }
};

