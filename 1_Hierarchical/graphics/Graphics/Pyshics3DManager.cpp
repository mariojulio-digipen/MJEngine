#include "Pyshics3DManager.h"
#include "FrameRateController.h"
#include "UIManager.h"
#include "Particle.h"
#include "Spring.h"
#include "TransformComponent.h"
#include <iostream>

extern FrameRateController* frameRateController;
extern UIManager* uiManager;

void Pyshics3DManager::Init()
{
	// initialize anything useful here
	Gravity = glm::vec3(0.0f, -9.8, 0.0f);
	dt = frameRateController->DeltaTime/10000.0f;
	//dampFactor = uiManager->DumpFactor;
	dampFactor = 0.30f;
	//std::thread physics = std::thread(&Pyshics3DManager::computeForcesSprings, this);
}

void Pyshics3DManager::Update()
{
	/*if (!threads_list_ready)
	{
		my_threads.reserve(springsNum);
		threads_list_ready = true;
	}*/

	// compute springs tensions before updating new positions of particles
	//int springVecSize = managedSprings.size();

	Spring** my_iter = &managedSprings[0]; // this is a pointer to an element of this vector (which has pointers)
	for (int i = 0; i < springsNum; i++)
	{
		float d, ext;
		glm::vec3 v;
		/*v = (*my_iter)->particle1->location -
			(*my_iter)->particle2->location;*/
		short id1 = (*my_iter)->particleId1;
		short id2 = (*my_iter)->particleId2;
		v = managedParticles[id1]->location -
			managedParticles[id2]->location;
		d = glm::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);//(float)v.length();
		ext = d - (*my_iter)->naturalLength;
		(*my_iter)->d = d;
		(*my_iter)->tension =
			((*my_iter)->k * ext);// / managedSprings[i]->naturalLength;
		//if (id1 == 0)
			(*my_iter)->tension *= 75.5f;
		my_iter++;
	}


	// Physics engine workflow

	// for each particle:
	for (int i = 0; i < particlesNum; i++)
	{
		Particle* particle = managedParticles[i];

		glm::vec3 nextLocation = glm::vec3(0.0f); // initialization
		glm::vec3 nextVelocity = glm::vec3(0.0f); // initialization
		float nextMass = particle->mass;

		// 0. Get the current location (and other properties) of the particle (no current rotation, does not apply for cloth)
		glm::vec3 currLocation = particle->location;
		glm::vec3 currVelocity = particle->velocity;

		if (particle->isFixed)
		{
			nextLocation = currLocation;
			nextVelocity = glm::vec3(0.0f);
		}
		else
		{
			// 1. Get the summation of all the forces of one particle (total momentum)
			// Ft = fg + fs + fd
			glm::vec3 Ft;
			// fg
			glm::vec3 fg = this->Gravity*particle->mass;

			// fs
			glm::vec3 fs;
			#pragma omp parallel for schedule(dynamic, 1) // Magic: Multi-thread y loop
			for (int k = 0; k < springsNum/*managedSprings.size()*/; k++)
			{
				Spring* spring = managedSprings[k];
				glm::vec3 dirV(0.0f);
				short id1 = spring->particleId1;
				short id2 = spring->particleId2;
				if (particle->id == spring->particleId1)
				{
					dirV = managedParticles[id2]->location - managedParticles[id1]->location;
					dirV = glm::normalize(dirV);
				}
				if (particle->id == spring->particleId2)
				{
					dirV = managedParticles[id1]->location - managedParticles[id2]->location;
					dirV = glm::normalize(dirV);
				}
				
				fs += spring->tension * dirV;
			}
			
			Ft = fg + fs;// +fw;

			// 2. Get the acceleration (integral of momentum) - constant for my project
			// P = acceleration = Ft / m 
			glm::vec3 acceleration = Ft / particle->mass; // mas is constant for this project

			// 3. Get the new velocity of one particle
			// C dot = velocity since mi = M
			// velocity = currentVelo + acceleration * delta;
			nextVelocity = currVelocity + acceleration * dt;

			// 4. damping velocity
			// velocity = velocity * dampFactor
			nextVelocity = nextVelocity * dampFactor;

			// 5. Get the new position for the particle (integral of x dot, which is equals to c dot in my project)
			// No rotation involved in my project
			// location = currentLocation + (velocity + currentVelo) * delta * 0.5;
			nextLocation = currLocation + 0.5f * (nextVelocity + currVelocity) * dt;

			// 6. Check collisions between the particle and other shapes (most likely only a sphere for this project)
			// TODO

			// 7. Pseudo collision with floor (y = 0 for this project)
			if (nextLocation.y <= 0.01f)
				nextLocation.y = 0.01f;

			// 8. Collision with the sphere
			float offset = 0.5f;
			glm::vec3 test_particle_location = nextLocation; // collidin particle
			glm::vec3 target_sphere_location = _targetTrans->GetPosition(); // my sphere center
			glm::vec3 target_sphere_scale = _targetTrans->GetScale(); //  my sphere diameter
			float radius = target_sphere_scale.x * 0.31f; // actual radius (uniform, it's a sphere)

			glm::vec3 spherecenter_to_particle = test_particle_location - target_sphere_location;
			float distance_sqrd = spherecenter_to_particle.x*spherecenter_to_particle.x +
				spherecenter_to_particle.y*spherecenter_to_particle.y +
				spherecenter_to_particle.z*spherecenter_to_particle.z;

			if (distance_sqrd < radius*radius)
			{
				glm::vec3 n_spherecenter_to_particle = glm::normalize(spherecenter_to_particle);
				n_spherecenter_to_particle = n_spherecenter_to_particle * (radius - glm::sqrt(distance_sqrd));
				nextLocation = nextLocation + n_spherecenter_to_particle;
			}
				
			//float sqrtLength_Particle = nextLocation.x*nextLocation.x + nextLocation.y*nextLocation.y + nextLocation.z*nextLocation.z;

			/*if (sqrtLength_Particle < 1.75f*1.75f*)
				nextLocation = glm::normalize(nextLocation)*1.75f*1.08f;*/


			particle->location = nextLocation;
			particle->velocity = nextVelocity;

		}

	}



	


	
}

void Pyshics3DManager::computeForcesSprings(int k, Particle* particle, glm::vec3* fs)
{
	Spring* spring = managedSprings[k];
	glm::vec3 dirV(0.0f);
	short id1 = spring->particleId1;
	short id2 = spring->particleId2;
	if (particle->id == spring->particleId1)
	{
		dirV = managedParticles[id2]->location - managedParticles[id1]->location;
		dirV = glm::normalize(dirV);
	}
	if (particle->id == spring->particleId2)
	{
		dirV = managedParticles[id1]->location - managedParticles[id2]->location;
		dirV = glm::normalize(dirV);
	}

	(*fs) += spring->tension * dirV;
}

void Pyshics3DManager::integrateRK4(float attribute, float t)
{
	
}

void Pyshics3DManager::ManageThisParticle(Particle* particle)
{
	if (!particle)
		std::cout << "particle is null!";

	managedParticles.push_back(particle);
	particlesNum++;
}

void Pyshics3DManager::ManageThisSpring(Spring* spring)
{
	if (!spring)
		std::cout << "particle is null!";

	managedSprings.push_back(spring);
	springsNum++;
}

// for rk4
//Derivative Pyshics3DManager::evaluateState(const State& initial, float t, float dt, const Derivative& d)
//{
//	State state;
//	state.xt = initial.xt + d.dx*dt; // Location -> p1 = p0 + velocity * delta
//	state.vt = initial.vt + d.dv*dt; // Velocity -> v1 = v0 + acc * delta
//
//	Derivative output;
//	output.dx = state.vt;
//	//output.dv = acceleration();
//	return output;
//}


// for rk4
//glm::vec3 Pyshics3DManager::acceleration(Particle* particle, const State& state, float t)
//{
//	//// gravity (fg)
//	//glm::vec3 fg = Gravity;
//
//	//// spring (fs)
//	//glm::vec3 fs = particle->GetTotalForce();
//
//
//	//return P_LinearMomentumNext;
//}
