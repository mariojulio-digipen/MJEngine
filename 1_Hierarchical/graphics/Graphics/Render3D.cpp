#include "Render3D.h"
#include "Shader.h"
#include "GameObject.h"
#include "SDL.h"
#include "InputManager.h"
#include "MouseManager.h"
#include "UIManager.h"

extern UIManager* uiManager;
extern SDL_Window* mainWindow;
extern InputManager* inputManager;
extern MouseManager* mouseManager;

Render3D::Render3D() : CurrentCamera(new Camera3D)
{
	// by default, create a gbuffer object
	int w, h;
	SDL_GetWindowSize(mainWindow, &w, &h);
	
	gBuffer = new GBuffer(w, h);
	gBuffer->Create();



}

Render3D::~Render3D()
{
	delete CurrentCamera;
	delete gBuffer;
}

void Render3D::InitCamera()
{
	CurrentCamera->MouseMove = mouseManager;
	CurrentCamera->InputMove = inputManager;
	CurrentCamera->ViewDirection = glm::vec3(0.206282, -0.496878, -0.842941);//glm::vec3(0.0f, 0.0f, -1.0f); //glm::vec3(-0.639008f, -0.386654f, -0.664957f);//
	CurrentCamera->Position = glm::vec3(-0.775346, 3.24137, 4.65617);//glm::vec3(0.0f, 5.0f, 20.1f); //glm::vec3(4.30213f, 2.83954f, 5.3389f);//
	CurrentCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f); 
	CurrentCamera->FovAngleDegrees = 45.0f;

}

void Render3D::updateCamera(int* width, int* height)
{
	CurrentCamera->Width = (*width);
	CurrentCamera->Height = (*height);
	CurrentCamera->Ratio = (float)(*width) / (*height);
	CurrentCamera->Update();
	/*std::cout << CurrentCamera->ViewDirection.x << "," << CurrentCamera->ViewDirection.y << ","
		<< CurrentCamera->ViewDirection.z << std::endl;*/
	/*std::cout << CurrentCamera->Position.x << "," << CurrentCamera->Position.y << ","
		<< CurrentCamera->Position.z << std::endl;*/
}

void Render3D::UseGBuffer()
{
	gBuffer->Bind();
}

void Render3D::StopGBuffer()
{
	gBuffer->UnBind();
}

void Render3D::Update()
{
	int w, h;
	SDL_GetWindowSize(mainWindow, &w, &h);
	updateCamera(&w, &h);

}

void Render3D::PushLight(BRDFLight light)
{
	this->lights.push_back(light);
}

void Render3D::RegularLightingUpdate()
{
	defaultShader->Disable2D();
	updateLights();
}

//void Render3D::GBufferPass()
//{
//	gBuffer->Bind();
//	gBufferShader
//}

void Render3D::DeferredLightingIntensity()
{
	GLint currentProgramId;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgramId);

	glm::vec3 LightIntensity = glm::vec3(0.2f);
	GLuint IntensityLoc = glGetUniformLocation(currentProgramId, "LightIntensity");
	glUniform3fv(IntensityLoc, 1, &LightIntensity[0]);
}




void Render3D::DeferredLightingPass()
{
	GLint currentProgramId;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgramId);

	GLint is2D = 1;
	GLint _2DLoc = glGetUniformLocation(currentProgramId, "Lights");
	glUniform1i(_2DLoc, is2D);
	updateLights();
}

void Render3D::updateLights()
{
	// this is lighting pass (old way, convert to deferred shading!!)
	// Send lights data to GPU






	int currentProgramId;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgramId);



	std::vector<BRDFLight>::iterator lightIt = lights.begin() + 1;
	int numLights = 0;
	
	std::string str;
	while (lightIt != lights.end())
	{
		// send position
		std::stringstream positionNameTMP;
		const char* positionName;
		glm::vec3 lightPosition = *(*lightIt).LightPosition;
		positionNameTMP << "GLights[" << numLights << "].LightPosition";
		str = positionNameTMP.str();
		positionName = str.c_str();

		GLuint posLoc = glGetUniformLocation(currentProgramId, positionName);
		glUniform3fv(posLoc, 1, &lightPosition[0]);

		// send ambient
		std::stringstream ambientNameTMP;
		const char* ambientName;
		glm::vec3 ambientLight = *(*lightIt).LightAmbient;
		ambientNameTMP << "GLights[" << numLights << "].LightAmbient";
		str = ambientNameTMP.str();
		ambientName = str.c_str();
		GLuint ambientLoc = glGetUniformLocation(currentProgramId, ambientName);
		glUniform3fv(ambientLoc, 1, &ambientLight[0]);

		// send intensity
		std::stringstream intensityNameTMP;
		const char* intensityName;
		glm::vec3 intensity = *(*lightIt).LightIntensity;
		intensityNameTMP << "GLights[" << numLights << "].LightIntensity";
		str = intensityNameTMP.str();
		intensityName = str.c_str();
		GLuint intensityLoc = glGetUniformLocation(currentProgramId, intensityName);
		glUniform3fv(intensityLoc, 1, &intensity[0]);

		// send color
		std::stringstream colorNameTMP;
		const char* colorName;
		glm::vec3 color;
		if (numLights > 1 && uiManager->LightsColors.size() > 0)
		{
			color.x = uiManager->LightsColors[numLights - 1].x;
			color.y = uiManager->LightsColors[numLights - 1].y;
			color.z = uiManager->LightsColors[numLights - 1].z;
		}
		//glm::vec3 color = *(*lightIt).LighColor;
		colorNameTMP << "GLights[" << numLights << "].LightColor";
		str = colorNameTMP.str();
		colorName = str.c_str();
		GLuint colorLoc = glGetUniformLocation(currentProgramId, colorName);
		glUniform3fv(colorLoc, 1, &color[0]);

		numLights++;
		lightIt++;
	}

	uiManager->LightsNumber = numLights;
	
	GLint lightnumLoc = glGetUniformLocation(currentProgramId, "lightnum");
	glUniform1i(lightnumLoc, numLights+1);
	
	glm::vec3 cameraPos = CurrentCamera->Position;
	GLint cameraPosLocation = glGetUniformLocation(currentProgramId, "CameraPos");
	glUniform3fv(cameraPosLocation, 1, &cameraPos[0]);


}



//bool Render3D::SendDataToGPU()
//{
//	cube->SendDataToGPU();
//	sphere->SendDataToGPU();
//	torus->SendDataToGPU();
//	plane->SendDataToGPU();
//	teapot->SendDataToGPU();
//
//	if (!defaultShader->Init())
//		return false;
//
//	
//
//	return true;
//}

//bool Render3D::CreateBuffers()
//{
//	cube = new Cube(true, false, defaultShader, CurrentCamera);
//	sphere = new Sphere(true, false, defaultShader, CurrentCamera);
//	torus = new Torus(true, false, defaultShader, CurrentCamera);
//	plane = new Plane(true, false, defaultShader, CurrentCamera);
//	teapot = new Teapot(true, false, defaultShader, CurrentCamera);
//
//	cube->CreateBuffers();
//	sphere->CreateBuffers();
//	torus->CreateBuffers();
//	plane->CreateBuffers();
//	teapot->CreateBuffers();
//
//
//	
//	return true;
//}

/*
bool RenderTest::ProcessShader()
{
	defaultShader->UseProgram();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return true;
}*/

/*void RenderTest::generateBuffers()
{
	glGenBuffers(VBOsNumber, vbo);
	glGenVertexArrays(VAOsNumber, vao);
}*/

/*void RenderTest::buildVBOs(int vboIndex, VBO_TYPE vboType)
{
	const int points = 12*3;
	const int floatsPerPoint = 3;
	const int floatsPerColor = 4;
	const int floatsPerUV = 2;


	glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex]);
	if (vboType == VERTEX)
	{
		const float modelData[points*floatsPerPoint] = {
			
			 1.0,  1.0,  1.0, // Top right front 
			 1.0, -1.0,  1.0, // Bottom right front 
			-1.0,  1.0,  1.0, // Top left front 

			 1.0, -1.0,  1.0, // Bottom right front
			-1.0,  1.0,  1.0, // Top left front
			-1.0, -1.0,  1.0, // Bottom left front

			-1.0, -1.0,  1.0, // Bottom left front
			-1.0,  1.0,  1.0, // Top left front
			-1.0, -1.0,  -1.0, // Bottom left back

			-1.0,  1.0,  1.0,  // Top left front
			-1.0, -1.0,  -1.0, // Bottom left back
			-1.0,  1.0,  -1.0, // Top left back

			-1.0, -1.0,  -1.0, // Bottom left back
			-1.0,  1.0,  -1.0, // Top left back
			 1.0,  1.0,  -1.0, // Top right back

			 1.0,   1.0,  -1.0, // Top right back
			 -1.0, -1.0,  -1.0, // Bottom left back
			 1.0,  -1.0,  -1.0, // Bottom right back

			 -1.0, -1.0,  -1.0, // Bottom left back
			  1.0, -1.0,  -1.0, // Bottom right back
			 -1.0, -1.0,   1.0, // Bottom left front

			  1.0, -1.0,  -1.0, // Bottom right back
			 -1.0, -1.0,   1.0, // Bottom left front
			  1.0, -1.0,   1.0, // Bottom right front 

			  1.0, -1.0, -1.0, // Bottom right back
			  1.0, -1.0,  1.0, // Bottom right front 
			  1.0,  1.0,  1.0, // Top right front 

			  1.0, -1.0, -1.0, // Bottom right back
			  1.0,  1.0,  1.0, // Top right front 
			  1.0,  1.0,  -1.0, // Top right back

			  1.0,  1.0,  1.0, // Top right front 
			  1.0,  1.0, -1.0, // Top right back
			 -1.0,  1.0, -1.0, // Top left back

			  1.0,  1.0,  1.0, // Top right front 
			 -1.0,  1.0, -1.0, // Top left back
			 -1.0,  1.0,  1.0, // Top left front

			
		};

		int sizeInBytes = (points * floatsPerPoint) * sizeof(float);
		glBufferData(GL_ARRAY_BUFFER, sizeInBytes, modelData, GL_STATIC_DRAW);
	}

	if (vboType == COLOR)
	{
		const float colors[points*floatsPerColor] = {
			1.0, 0.0, 0.0, 1.0, // Top right front 
			0.0, 1.0, 0.0, 1.0,	// Bottom right front 
			0.0, 0.0, 1.0, 1.0, // Top left front 

			0.0, 1.0, 0.0, 1.0, // Bottom right front
			0.0, 0.0, 1.0, 1.0, // Top left front
			1.0, 1.0, 1.0, 1.0, // Bottom left front

			1.0, 1.0, 1.0, 1.0, // Bottom left front
			0.0, 0.0, 1.0, 1.0, // Top left front
			0.0, 1.0, 0.0, 1.0, // Bottom left back

			0.0, 0.0, 1.0, 1.0, // Top left front
			0.0, 1.0, 0.0, 1.0, // Bottom left back
			1.0, 0.0, 0.0, 1.0, // Top left back

			0.0, 1.0, 0.0, 1.0, // Bottom left back
			1.0, 0.0, 0.0, 1.0, // Top left back
			0.0, 0.0, 1.0, 1.0, // Top right back

			0.0, 0.0, 1.0, 1.0, // Top right back
			0.0, 1.0, 0.0, 1.0, // Bottom left back
			0.0, 1.0, 0.0, 1.0, // Bottom right back

			0.0, 1.0, 0.0, 1.0, // Bottom left back
			0.0, 1.0, 0.0, 1.0, // Bottom right back
			1.0, 1.0, 1.0, 1.0, // Bottom left front

			0.0, 1.0, 0.0, 1.0, // Bottom right back
			1.0, 1.0, 1.0, 1.0, // Bottom left front
			0.0, 1.0, 0.0, 1.0,	// Bottom right front 

			0.0, 1.0, 0.0, 1.0, // Bottom right back
			0.0, 1.0, 0.0, 1.0,	// Bottom right front 
			1.0, 0.0, 0.0, 1.0, // Top right front

			0.0, 1.0, 0.0, 1.0, // Bottom right back
			1.0, 0.0, 0.0, 1.0, // Top right front
			0.0, 0.0, 1.0, 1.0, // Top right back

			1.0, 0.0, 0.0, 1.0, // Top right front
			0.0, 0.0, 1.0, 1.0, // Top right back
			1.0, 0.0, 0.0, 1.0, // Top left back

			1.0, 0.0, 0.0, 1.0, // Top right front
			1.0, 0.0, 0.0, 1.0, // Top left back
			0.0, 0.0, 1.0, 1.0, // Top left front

		};

		int sizeInBytes = (points * floatsPerColor) * sizeof(float);
		glBufferData(GL_ARRAY_BUFFER, sizeInBytes, colors, GL_STATIC_DRAW);
	}

	if (vboType == UV)
	{
		const float uv[points*floatsPerUV] = {

			//front face
			1.0, 1.0, // Top right front 
			1.0, 0.0, // Bottom right front 
			0.0, 1.0, // Top left front 

			1.0, 0.0, // Bottom right front
			0.0, 1.0, // Top left front
			0.0, 0.0, // Bottom left front

			//left face
			1.0, 0.0, // Bottom left front
			1.0, 1.0, // Top left front
			0.0, 0.0, // Bottom left back

			1.0, 1.0,  // Top left front
			0.0, 0.0, // Bottom left back
			0.0, 1.0, // Top left back

			//back face
			1.0, 0.0, // Bottom left back
			1.0, 1.0, // Top left back
			0.0, 1.0, // Top right back

			0.0, 1.0, // Top right back
			1.0, 0.0, // Bottom left back
			0.0, 0.0, // Bottom right back

			//bottom face
			0.0, 0.0, // Bottom left back
			1.0, 0.0, // Bottom right back
			0.0, 1.0, // Bottom left front

			1.0, 0.0, // Bottom right back
			0.0, 1.0, // Bottom left front
			1.0, 1.0, // Bottom right front 

			//right face
			1.0, 0.0, // Bottom right back
			0.0, 0.0, // Bottom right front 
			0.0, 1.0, // Top right front 

			1.0, 0.0, // Bottom right back
			0.0, 1.0, // Top right front 
			1.0, 1.0, // Top right back

			//top face
			1.0, 0.0, // Top right front 
			1.0, 1.0, // Top right back
			0.0, 1.0, // Top left back

			1.0, 0.0, // Top right front 
			0.0, 1.0, // Top left back
			0.0, 0.0 // Top left front

		};

		int sizeInBytes = (points * floatsPerUV) * sizeof(float);
		glBufferData(GL_ARRAY_BUFFER, sizeInBytes, uv, GL_STATIC_DRAW);
	}
	
}*/

/*void RenderTest::buildVBOsFromOBJ(int vboIndex, VBO_TYPE vboType)
{
	if (vboType == VERTEX)	
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex]);
		VertexData* vertexData = cubeLoader->GetVertexData();
		int sizeInBytes = (vertexData->numberOfData) * sizeof(float);
		glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &vertexData->dataArray[0], GL_STATIC_DRAW);
		return;
	}

	if (vboType == UV)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex]);
		VertexData* textureData = cubeLoader->GetTexturesData();
		int sizeInBytes = (textureData->numberOfData) * sizeof(float);
		glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &textureData->dataArray[0], GL_STATIC_DRAW);
		return;
	}

	if (vboType == NORMAL)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIndex]);
		VertexData* normalData = cubeLoader->GetVertexData();
		int sizeInBytes = (normalData->numberOfData) * sizeof(float);
		glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &normalData->dataArray[0], GL_STATIC_DRAW);
		return;
	}

}*/

/*void RenderTest::buildVAOs(int vaoIndex)
{
	glBindVertexArray(vao[vaoIndex]);
}*/

/*void RenderTest::buildIBOFromOBJ()
{
	int sizeInBytes = 0;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	std::vector<int> iboData = cubeLoader->getIndexBufferData();
	sizeInBytes = iboData.size() * sizeof(int);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeInBytes, &iboData[0], GL_STATIC_DRAW);

}*/

