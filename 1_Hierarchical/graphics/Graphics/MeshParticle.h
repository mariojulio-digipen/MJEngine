#pragma once

#include "Particle.h"
#include <glm.hpp>

class MeshParticle : public Particle
{
private:
	glm::mat4 modelMatrix;

	// inherits location from parent
	float rotationAngle;
	glm::vec3 size;

	// the matrices (inherits location from parent)
	glm::mat4 rotate;
	glm::mat4 scale;

public:

	// Set Rotation
	void SetRotation(const float& angle) 
	{ 
		//rotationAxis = rot; 
		rotationAngle = angle; 
		rotate = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	glm::mat4 GetRotationMatrix() { return rotate; }
	
	// Set Scale
	void SetScale(const glm::vec3& scale) 
	{  
		size = scale;
		this->scale = glm::scale(glm::mat4(1.0f), glm::vec3(size));
	}
	glm::mat4 GetSCaleMatrix() { return scale; }
	

	// Provides the model matrix, to be updated every frame 
	// (just like in transform)
	glm::mat4 GetModelMatrix() 
	{
		modelMatrix = translate * rotate * scale;
		return modelMatrix; 
	}

	MeshParticle();
	~MeshParticle();
};

