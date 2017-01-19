#pragma once
#include "Particle.h"
#include <glm.hpp>

class Spring
{
public:
	glm::vec3 fs;

	
	float k; // spring coefficient
	float d; // spring displacement
	float naturalLength; // the length in normal state
	float tension;
	int id;

	// Spring is holding 2 particles
	/*Particle* particle1;
	Particle* particle2;*/
	short particleId1;
	short particleId2;

	
	
	

	

	// debug data
	//glm::vec2 p1GridPos, p2GridPos;

	Spring();
	~Spring();
};

