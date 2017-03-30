#pragma once

#include <glm.hpp>
#include <vector>
#include <gtc\matrix_transform.hpp>

class Particle
{

public:
	// physical properties for particles
	glm::vec3 location; // qi
	glm::mat4 translate;

	glm::vec3 velocity;
	glm::vec3 normal;

	float mass; // mi
	bool isFixed; // is held or not
	short id;
	



	virtual void SetLocation(const glm::vec3& loc);
	virtual void SetVelocity(const glm::vec3& vel);

	glm::vec3 GetVelocity() { return velocity; }
	glm::vec3 GetLocation() { return location; }
	glm::mat4 GetTranslationMatrix() { return translate; }
	

	Particle();
	virtual ~Particle();
};

