#pragma once

#include <glm.hpp>
#include <vector>

class Spring;
class Particle
{
private:
	//std::vector<Spring*> attachedSprings;
	//glm::vec3 totalForce;

public:

	glm::vec3 location; // qi
	glm::vec3 velocity;
	glm::vec3 normal;

	float mass; // mi
	bool isFixed; // is held or not
	short id;
	

	Particle();
	~Particle();

	//void SetTotalForce(glm::vec3 tForce) { this->totalForce = tForce; }
	//glm::vec3 GetTotalForce() { return this->totalForce; }

	//void attachSpring(Spring* spring) { attachedSprings.push_back(spring); }
};

