#pragma once

#include <glm.hpp>
#include <vector>
//#include <thread>

// physics manager 3D only supports physics for particles for now 
// (for the animation assignment purposes)
// next version will support game objects support.

struct State
{
	glm::vec3 xt; // position
	glm::vec3 vt; // velocity
};

struct Derivative
{
	glm::vec3 dx; // dx/dt - velocity
	glm::vec3 dv; // dv/dt - acceleration
};



class Particle;
class Spring;
class TransformComponent;
class Pyshics3DManager
{

private:
	//bool threads_list_ready = false;
	//std::vector<std::thread> my_threads;
	
	float dt;
	float dampFactor;

	std::vector<Particle*> managedParticles;
	std::vector<Spring*> managedSprings;
	int particlesNum;
	int springsNum;

	TransformComponent* _targetTrans;


	void integrateRK4(float attribute, float t);


	void computeForcesSprings(int k, Particle* particle, glm::vec3* fs);
	//std::thread spawn(int k, Particle* particle, glm::vec3* fs) {
	//	//return std::thread(&Pyshics3DManager::computeForcesSprings(k, particle, fs));
	//	return std::thread([this, k, particle, fs] { this->computeForcesSprings(k, particle, fs); });
	//}

public:
	Pyshics3DManager() {}
	~Pyshics3DManager() {}

	float h;
	glm::vec3 Gravity;

	void Init();
	void Update();
	//Derivative evaluateState(const State& initial, float t, float dt, const Derivative& d);
	//glm::vec3 acceleration(Particle* particle, const State& state, float t);

	void ManageThisParticle(Particle* particle);
	void ManageThisSpring(Spring* spring);


	// "hardcoded" way to find a sphere.
	// I only have one sphere and I send it from wherever object component I want to test collision
	// works only for one target sphere
	void SetTestTargeTrans(TransformComponent* trans) { _targetTrans = trans; }


};

