#pragma once

#include <glew.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>	

#include "Shader.h"
#include "Camera2D.h"
#include "GLObject.h"
#include "ModelLoader.h"

class UIQuad;

class Render2D
{


private:
	Shader* defaultShader;

	UIQuad* quad;


public:

	Camera2D* CurrentCamera;

	Render2D() :
		CurrentCamera(new Camera2D)
	{
	}

	~Render2D()
	{
		delete CurrentCamera;
	}

	void AttachShader(Shader* shader) { defaultShader = shader; }
	void Use();

	void InitCamera();
	void UpdateCamera(int* width, int* height);


};

