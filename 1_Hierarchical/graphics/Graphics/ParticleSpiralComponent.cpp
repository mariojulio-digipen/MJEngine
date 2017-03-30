// ParticleSpiralComponent.CPP
// Implementation autogenerated by MJEngine automation tools
// WARNING: Do not modify autogenerated comments, automation 
// tools depend on them.

// Edition author: mzaldivar

#include "ParticleSpiralComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "MeshParticle.h"
#include "ResourceManager.h"
#include "FrameRateController.h"
#include "LightPointComponent.h"
#include "Lighter.h"
#include "Shader.h"
#include "UIManager.h"
#include <sstream>

#define PI 3.14159265359f

extern ResourceManager* resourceManager;
extern FrameRateController* frameRateController;
extern Shader* meshParticleShader;
extern Shader* shader;
extern UIManager* uiManager;

ParticleSpiralComponent::ParticleSpiralComponent() :
	currentParticles(0), maxParticles(0), systemAngle(0.0f)
{
}


ParticleSpiralComponent::~ParticleSpiralComponent()
{
}


bool ParticleSpiralComponent::Init()
{
	// TODO - Implement initializations here
	// This function runs once when object manager
	// load objects, before serialization
	uiManager->SpiralActive = GetOwner()->IsAlive;
	//maxParticles = 100;
	//currentParticles = 0;
	//lifeSpan = 4;
	//sigma = 2.0f;
	u = glm::vec3(0.0f, 1.0f, 0.0f);
	u = glm::normalize(u);
	trans = GetOwner()->FindComponent<TransformComponent>();
	return true;
}


void ParticleSpiralComponent::Update()
{
	// TODO - Implement all the updates here
	// This function is executed once everyframe

	

	this->h = 0.01f;
	this->systemAngle += PI / 8.0f;

	//trans->SetRotation(glm::vec3(0.0f, systemAngle, 0.0f));

	updateFromUI();

	if (randomLocationFactor < maxParticles)
		randomLocationFactor = currentParticles;
	else
		randomLocationFactor = 0;
		
	particleSystem->SetCurrentParticles(currentParticles); // will manipulated through the uimanager
	
	if (!uiManager->SpiralActive)
	{
		updateModelMatrices();
	}
	else
	{
		updateParticleContainer();
		updateModelMatrices();
	}
	
	
	shader->UnUseProgram();
	int shaderUsed = meshParticleShader->GetProgramID();//particleSystem->GetShaderUsed();
	meshParticleShader->UseProgram(); // particle system uses mesh shader

	// updates lighting
	Lighter lighter;
	lighter.ProcessLights(shaderUsed, light);


	particleSystem->Draw();


	shader->UseProgram();

}

void ParticleSpiralComponent::updateFromUI()
{
	maxParticles = uiManager->SpiralMaxParticles;
	lifeSpan = uiManager->SpiralLifeSpan;
	sigma = uiManager->SpiralSigma;
}

void ParticleSpiralComponent::updateModelMatrices()
{
	// update the model matrix
	// for each particle
	std::vector<glm::mat4> models;
	for (int i = 0; i < currentParticles; i++)
	{
		MeshParticle* particle = particlesContainer[i];
		glm::mat4 newModelMatrix;

		glm::vec3 xt = particle->GetLocation();			// x(t) previous position
		glm::vec3 vt = particle->GetVelocity();			// v(t) previous velocity

		// use split
		glm::vec3 avt = vt + splittedAccelerationOperators(vt, xt);// computeSpiralAcceleration(vt);//computeObitAcceleration(xt);
		glm::vec3 xth = computeNewPosition(xt, avt);		// x(t+h) new position

		// make the translation matrix
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), xth);

		// and this will be the model matrix
		glm::mat4 prevScaleMatrix = particle->GetSCaleMatrix();
		newModelMatrix = translate * prevScaleMatrix;
		models.push_back(newModelMatrix);

		// update previous location with current
		particle->SetLocation(xth);

		// update previous velocity with current
		glm::vec3 vth;
		if(uiManager->AccelIncrease)
			vth = computeNewVelocity(avt);
		else
			vth = computeNewVelocity(vt);
		particle->SetVelocity(vth);
		
	}

	if(static_cast<int>(models.size()) > 0)
		particleSystem->SetModelMatrices(&models[0], currentParticles);
}

void ParticleSpiralComponent::updateParticleContainer()
{
	long long int frame = static_cast<long long int>(frameRateController->GetFrameNumber()) ;

	if ((frame % lifeSpan) == 0)
	{
		// every n frames, the system will emit a new particle
		glm::vec3 location;
		glm::vec3 velocity;
		float angle;
		float scale;

		if (uiManager->CoolEffect)
			spawnRandomLocation(location);
		else
			spawnDefaultLocation(location);
		spawnDefaultVelocity(velocity);
		spawnDefaultAngle(angle);
		spawnDefaultScale(scale);
	

		manageLifeCycle(location, velocity, angle, scale);

	}
}

void ParticleSpiralComponent::spawnDefaultVelocity(glm::vec3& velocity)
{
	float xv = uiManager->SpiralSpawnVelocity.x;//20.0f;
	float yv = uiManager->SpiralSpawnVelocity.y;//10.0f;
	float zv = uiManager->SpiralSpawnVelocity.z; //1.0f;
	velocity = glm::vec3(xv, yv, zv);
}

void ParticleSpiralComponent::spawnDefaultAngle(float& angle)
{
	angle = 0.0f;
}

void ParticleSpiralComponent::spawnDefaultScale(float& scale)
{
	scale = 0.5f;//(rand() % 200) / 100.0f + 0.05f;
}

void ParticleSpiralComponent::spawnDefaultLocation(glm::vec3& location)
{
	float xl = 1.0f;
	float yl = 1.0f;
	float zl = 1.0f;
	location = glm::vec3(xl, yl, zl) + trans->GetPosition();
}

void ParticleSpiralComponent::spawnRandomLocation(glm::vec3& location)
{
	float radius = 5.0;
	float offset = 1.5f;
	GLfloat angle = (GLfloat)randomLocationFactor / (GLfloat)maxParticles * 360.0f;

	GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
	float x = sin(angle) * radius + displacement;
	displacement = (rand() % (GLint)(16 * offset * 100)) / 100.0f - offset;
	float y = displacement * 0.4f;
	displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
	float z = cos(angle) * radius + displacement;
	location = glm::vec3(x, y, z) + trans->GetPosition();
}

void ParticleSpiralComponent::manageLifeCycle(glm::vec3& location, 
	glm::vec3& velocity, float& angle, float& scale)
{
	// beforehand, we need to validate if the container
	// is already full.=
	if (currentParticles >= maxParticles)
	{
		// if the container is full, just pop back and delete that 
		// particle. Then, push front a new particle.
		MeshParticle* particle = particlesContainer.back();
		particlesContainer.pop_back();

		particle->SetLocation(location);
		particle->SetVelocity(velocity);
		particle->SetRotation(angle);
		particle->SetScale(glm::vec3(scale));
		particlesContainer.insert(particlesContainer.begin(), particle);

	}
	else
	{
		// only create particles if the container is not full
		MeshParticle* particle = new MeshParticle;
		particle->SetLocation(location);
		particle->SetVelocity(velocity);
		particle->SetRotation(angle);
		particle->SetScale(glm::vec3(scale));
		particlesContainer.push_back(particle);

		++currentParticles;
	}
}

void ParticleSpiralComponent::Destroy()
{
	// TODO - Optional, use to release memory
	// This function is called by the user.
	for (int i = 0; i < currentParticles; i++)
		if(particlesContainer[i])
			delete particlesContainer[i];
		
	particlesContainer.clear();

	delete particleSystem;
}


void ParticleSpiralComponent::Serialize(std::istringstream* objcontentStream)
{
	std::string line;
	std::getline((*objcontentStream), line);

	std::istringstream lineStream(line.c_str());
	std::string word;

	while ((*objcontentStream) >> word)
	{
		if (word == "#")
		{
			std::getline((*objcontentStream), line);
			continue;
		}
		
		// TODO - Implement more if statements according to the file format
		// This function runs right after the Init function
		if (word == "amount")
		{
			std::getline((*objcontentStream), line);
			maxParticles = atoi(line.c_str());
			uiManager->SpiralMaxParticles = maxParticles;
		}
		
		if (word == "texture")
		{
			std::getline((*objcontentStream), line);
			line.erase(line.begin());
			textureImage = line;
		}

		if (word == "lifespan")
		{
			std::getline((*objcontentStream), line);
			lifeSpan = atoi(line.c_str());
			uiManager->SpiralLifeSpan = lifeSpan;
		}

		if (word == "sigma")
		{
			std::getline((*objcontentStream), line);
			sigma = static_cast<float>(atof(line.c_str()));
			uiManager->SpiralSigma = sigma;
		}
	}
}

void ParticleSpiralComponent::Start()
{
	particleSystem = new ParticleSystem;
	particleSystem->SetMaxParticles(maxParticles); // constant
	particleSystem->SetTextureImage(textureImage.c_str());
	particleSystem->Init();

	GameObject* obj = resourceManager->GetGameObjectByName("Sphere");
	light = obj->FindComponent<LightPointComponent>();



	GameObject* blackhole = resourceManager->GetGameObjectByName("BlackHole");
	transBH = blackhole->FindComponent<TransformComponent>();


	srand(static_cast <unsigned> (time(0)));
}

void ParticleSpiralComponent::CompleteExtraSetup()
{
	// TODO - Implement any setup that needs to be executed
	// after the owner gameobject already exists.
}


void ParticleSpiralComponent::HandleEvent(const Event* const evt)
{
	// TODO - Implement reactions to different events
}

glm::vec3 ParticleSpiralComponent::computeNewPosition(glm::vec3 prevPos, glm::vec3 prevVelo)
{
	glm::vec3 xth;
	
	float angle = sigma * h;
	float sinSigma = (glm::sin(angle) / sigma);
	float cosSigma = (1 - glm::cos(angle)) / sigma;
	glm::vec3 dotu = glm::dot(u, prevVelo) * u;
	glm::vec3 crossu = glm::cross(u, prevVelo);

	xth = prevPos + sinSigma*prevVelo + (h - sinSigma)*dotu + cosSigma*crossu;
	
	return xth;
}

glm::vec3 ParticleSpiralComponent::computeNewVelocity(glm::vec3 prevVelo)
{
	glm::vec3 vth;


	float angle = sigma * h;
	glm::vec3 dotu = glm::dot(u, prevVelo) * u;
	glm::vec3 crossu = glm::cross(u, prevVelo);

	vth = glm::cos(angle)*prevVelo + (1 - glm::cos(angle))*dotu + glm::sin(angle)*crossu;

	return vth;
}

glm::vec3 ParticleSpiralComponent::computeSpiralAcceleration(glm::vec3& prevVelo)
{
	glm::vec3 at;
	at = glm::cross(sigma*u, prevVelo);
	return at;
}

glm::vec3 ParticleSpiralComponent::computeObitAcceleration(glm::vec3& location)
{
	glm::vec3 acc;

	float g = uiManager->BHGravity;//29.8f;
	float mass = uiManager->BHMass;//50.0f;
	glm::vec3 O = transBH->GetPosition();//glm::vec3(0.0f, 5.0f, 0.0f);//trans->GetPosition();
	glm::vec3 P = location;
	float radius = glm::length(O - P);

	if (radius < 1.0f)
		return glm::vec3(0.0f);

	acc = g * mass * ((O - P) / glm::pow(radius, 3));

	return acc;
}

glm::vec3 ParticleSpiralComponent::splittedAccelerationOperators(glm::vec3& prevVelo, glm::vec3& location)
{
	// only 2 operators this time
	// a1 = spiral acceleration
	// a2 = orbit acceleration

	// step 1
	glm::vec3 ut = prevVelo * uiManager->DumpingFactor;  //0.001 - 0.01
	glm::vec3 uth1 = ut + h*computeObitAcceleration(location);

	// step 2
	glm::vec3 ut2 = uth1;
	glm::vec3 uth2 = ut2 + h*computeSpiralAcceleration(prevVelo);


	return uth2;
}