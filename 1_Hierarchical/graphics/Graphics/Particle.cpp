#include "Particle.h"



Particle::Particle()
{
}


Particle::~Particle()
{
}


void Particle::SetLocation(const glm::vec3& loc)
{
	location = loc;
	translate = glm::translate(glm::mat4(1.0f), location);
}

void Particle::SetVelocity(const glm::vec3 & vel)
{
	velocity = vel;
}