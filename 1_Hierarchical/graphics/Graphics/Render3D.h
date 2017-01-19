#pragma once

#include <glew.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>	

#include "Shader.h"
#include "Camera3D.h"
#include "GLObject.h"
#include "ModelLoader.h"

#include "Cube.h"
#include "Sphere.h"
#include "Torus.h"
#include "Plane.h"
#include "Teapot.h"

#include "GBuffer.h"
#include "FBO.h"

struct BRDFLight
{
	glm::vec3* LightPosition;
	glm::vec3* LightAmbient;
	glm::vec3* LightIntensity;
	glm::vec3* LighColor;
};

class Render3D
{


private:
	Shader* defaultShader;
	Shader* gBufferShader;
	Shader* simpleBuffer;

	GBuffer* gBuffer;


	void updateLights();
	void updateCamera(int* width, int* height);
	std::vector<BRDFLight> lights;

public:

	Camera3D* CurrentCamera;

	Render3D();

	~Render3D();

	void AttachShader(Shader* shader)
	{ 
		defaultShader = shader; 
	}

	void InitCamera();
	void Update();
	void UseGBuffer();
	void StopGBuffer();
	GBuffer* GetGBufferObject() { return gBuffer; }
	void PushLight(BRDFLight light);


	void RegularLightingUpdate();
	void DeferredLightingPass();
	void DeferredLightingIntensity();


};

