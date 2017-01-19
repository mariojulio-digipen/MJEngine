#pragma once

#include <glm.hpp>

class Event
{

public:
	Event();
	virtual ~Event();

};

class Contact;
class CollisionEvent : public Event
{
public:
	CollisionEvent() : C(0) {}
	~CollisionEvent() {}

	Contact* C;
};


class StartOrbit : public Event
{
public:
	StartOrbit() {}
	~StartOrbit() {}

	float AngularVelocity;
};

class DummyEvent : public Event
{
public:
	DummyEvent() {}
	~DummyEvent() {}

};

class SetTextureEvent : public Event
{
public:
	SetTextureEvent() {}
	~SetTextureEvent() {}

	int TextureUnit;
};

class Spline;
class StartMovingAlongPath : public Event
{
private:
	Spline* spline;

public:
	StartMovingAlongPath() {}
	~StartMovingAlongPath() {}

	void SetPath(Spline* spline) { this->spline = spline; }
	Spline* GetPath() const { return spline; }
};

class StopMovingAlongPath : public Event
{
private:
	/*glm::vec3 location;
	glm::vec3 rotation;
	glm::vec3 scale;*/

public:
	StopMovingAlongPath() {}
	~StopMovingAlongPath() {}

	/*void SetLastLocation(glm::vec3 location) { location = this->location; }
	void SetLastRotation(glm::vec3 rotation) { rotation = this->rotation; }
	void SetLastScale(glm::vec3 scale) { rotation = this->scale; }

	glm::vec3 GetStopLocation() const { return location; }
	glm::vec3 GetStopRotation() const { return rotation; }
	glm::vec3 GetStopScale() const { return scale; }*/

};

class GameObject;
class CreateControlPoint: public Event
{
private:
	GameObject* point;

public:
	CreateControlPoint() {}
	~CreateControlPoint() {}

	void SetPoint(GameObject* point) { this->point = point; }
	GameObject* GetPoint() const { return point; }

};


class DeleteControlPoints : public Event
{
private:

public:
	DeleteControlPoints() {}
	~DeleteControlPoints() {}

};

class IKGrabTarget : public Event
{
private:

public:
	IKGrabTarget() {}
	~IKGrabTarget() {}
};

class IKCCDResetIterations : public Event
{
private:

public:
	IKCCDResetIterations() {}
	~IKCCDResetIterations() {}
};