// RayPlaneIntersectEvaluatorComponent.H
// Header autogenerated by MJEngine automation tools
// WARNING: Do not modify autogenerated comments, automation 
// tools depend on them.

// Edition author: mzaldivar

#pragma once
#include "Component.h"
#include <vector>
#include <glm.hpp>

class Camera3D;
class TransformComponent;
class RayPlaneIntersectEvaluatorComponent : public Component
{
private:
	// Start Private properties list

		// TODO - declare private members
	TransformComponent* modelRootTrans;
	Camera3D* currentCamera;
	TransformComponent* trans;
	std::vector<glm::vec3> intersectionPoints;
	// End Private properties list
	
	// Start private user defined functions

		// TODO - declare user defined private functions

	// End private user defined functions

public:

	// Start Public properties list

		// TODO - declare public members

	// End Private properties list

	RayPlaneIntersectEvaluatorComponent();
	~RayPlaneIntersectEvaluatorComponent();

	// Start implementation functions
	bool Init();
	void Update();
	void HandleEvent(const Event* const evt);
	void Destroy();
	// End implementation functions

	// Start public user defined functions

		// TODO - declare user defined public functions
	std::vector<glm::vec3>* GetIntersectionPoints() { return &intersectionPoints; }
	void DeleteIntersectionPoints();

	// End public user defined functions
};
