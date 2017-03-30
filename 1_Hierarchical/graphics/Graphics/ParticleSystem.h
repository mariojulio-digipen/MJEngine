#pragma once

#include <glew.h>
#include <glm.hpp>
#include <vector>
#include <string>

#include "Particle.h"
#include "Shader.h"

class Event;
class Texture;
class ParticleSystem
{
private:

	// from obj model, get geometry info
	int vertexAttributeIndex; // this is the base to create instances
	int textureAttributeIndex;
	int normalAttributeIndex;
	

	int instanceMatrixAttributeIndex;
	//int positionSizeAttributeIndex;

	//// optional
	//int colorAttributeIndex;


	std::vector<GLfloat> vertexData;
	int vertexDataNumber;

	std::vector<GLfloat> textureData;
	int textureDataNumber;

	std::vector<GLfloat> normalData;
	int normalDataNumber;

	bool contextReady;
	

	//GLfloat* gpuParticlePosSizeData; 
	std::vector<glm::mat4> modelMatrices;
	int maxParticles;
	int numParticles; // for each particle in the system
	//GLfloat* gpuParticleColorData;
	//GLfloat* gpuParticleTextureData;
	//GLfloat* gpuParticleNormalData;

//	Shader* meshParticleShader;
	Texture* texture;
	std::string textureFile;

	// the base geometry to be shared
	GLuint vertexVBO;
	GLuint textureVBO;
	GLuint normalVBO;

	// per each - instanced
	GLuint modelMatrixVBO; // this is the instance matrix per each particle
	GLuint eachParticleVAO; // need 1 VAO per each particle to be drawn
	
	//GLuint positionSizeVBO;
	//GLuint colorVBO; // optional

	void createTexture();
	void createGLBuffers();
	void deleteGLBuffers();

	void buildParticleVBOs();
	void bindAttributes();
	void updateVBOs();
	void drawParticle();

	void fillGPUParticleData();

public:

	void SetMaxParticles(const int& max) { maxParticles = max; }
	int GetMaxParticles() { return maxParticles; }

	void SetCurrentParticles(const int& curr) { numParticles = curr; }
	int GetCurrentParticles() { return numParticles; }

	void SetModelMatrices(glm::mat4* modelMatrices);
	void SetModelMatrices(glm::mat4* modelMatrices, int amount);
	std::vector<glm::mat4> GetModelMatrices();

	
	
	glm::mat3 SpiralRotationMatrix(float angle, glm::vec3 u);
	float VortexRotationAngle(glm::vec3 xt, glm::vec3 vo, glm::vec3 u, float K, float T, float timeStep);
	float SpiralRotationAngle(float sigma, float timeStep);
	void SetTextureImage(const char* fileName);

	void Init();
	void Draw();

	

	ParticleSystem();
	~ParticleSystem();
};

