#define GL3_PROTOTYPES 1
#include <glew.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>


#include <gtx\transform.hpp>

#include <gtc\matrix_transform.hpp>
#include <gtx\matrix_decompose.hpp>
#include <gtc\type_ptr.hpp>
#include <gtx\euler_angles.hpp>
#include <glm.hpp>

#include "VideoBuffer.h"
#include "MouseManager.h"
#include "ModelLoader.h"
#include "FrameRateController.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "RenderManager.h"
#include "Render2D.h"
#include "Render3D.h"
#include "Shader.h"
#include "PhysicsManager.h"
#include "Pyshics3DManager.h"
#include "EventManager.h"
#include "UIManager.h"
#include "VQM.h"

// test quaternions
#include "Quaternion.h"
#include "VQS.h"


// move fbo functionality to render3d class
#include "FBO.h"

int input = 0;
std::string programName = "MY OpenGL Framework - 60 FPS";



int SCREEN_WIDTH = 1600;
int SCREEN_HEIGHT = 900;

SDL_GLContext mainContext;

SDL_Window* mainWindow = NULL;

Shader* shader = NULL;
Shader* particleShader = NULL;
Shader* meshParticleShader = NULL;
Shader* oceanShader = NULL;

VideoBuffer* buffer = NULL;

FBO* globalLightFBO = NULL;

RenderManager* renderManager = NULL;
MouseManager* mouseManager = NULL;
InputManager* inputManager = NULL;
FrameRateController* frameRateController = NULL;
ResourceManager* resourceManager = NULL;
PhysicsManager* physicsManager = NULL;
Pyshics3DManager* physics3DManager = NULL;
EventManager* eventManager = NULL;
UIManager* uiManager = NULL;

// imgui
GLuint quadVAO = 0;
GLuint quadVBO;

GLuint quadDebugVAO = 0;
GLuint quadDebugVBO;

GLuint quadLightVAO = 0;
GLuint quadLightVBO;

GLuint cubeVAO = 0;
GLuint cubeVBO;

bool Init();
bool SetOpenGLAttributes();
void PrintSDL_GL_Attributes();
void CheckSDLError(int line);
void RunGame();
void Cleanup();
bool TestColors(SDL_Event event);
void RenderStuff();

void RenderQuadAmbient();
void RenderQuadLight();
void RenderQuad_Debug();
void ReadHomeworkConfig();
void RunAnimationHomework1();
std::string readconfigfile(const char* fileName);
int homework = 0;



typedef std::vector<std::vector<int>> Matrix;

Matrix makeMatrixZeroes(int M, int N)
{
	Matrix m;
	for (size_t i = 0; i < N; i++)
	{
		std::vector<int> rows(M);
		m.push_back(rows);
	}

	return m;
}

Matrix makeMatrix(int M, int N)
{
	Matrix m;
	for (size_t i = 0; i < N; i++)
	{
		std::vector<int> rows(M);
		m.push_back(rows);
	}

	for (size_t i = 0; i < M; ++i)
	{
		for (size_t j = i + 1; j < N; ++j)
		{
			m[i][j] = 1;
		}

	}

	return m;
}

Matrix multiplyMatrices(Matrix A, Matrix B)
{
	int sizeM = A.size();
	int sizeN = B[0].size();      //rows, cols
	Matrix mres = makeMatrixZeroes(sizeM, sizeN);

	for (size_t k = 0; k < sizeN; k++)
	{
		for (size_t i = 0; i < sizeM; i++)
		{
			for (size_t j = 0; j < sizeN; j++)
			{
				mres[k][i] += A[k][j] * B[j][i];
			}
		}
	}

	return mres;
}

void printMatrix(Matrix A)
{
	int size = A[0].size();
	for (size_t c = 0; c < size; c++) { std::cout << " --- "; }

	for (size_t i = 0; i < A.size(); ++i)
	{
		
		std::cout << std::endl;
		std::cout << "| ";
		for (size_t j = 0; j < A[i].size(); ++j)
		{
			std::cout << A[i][j] << "   ";
		}
		std::cout << std::endl;

	}
	std::cout << std::endl;
	std::cout << std::endl;

}


int main(int argc, char *argv[])
{
	// testing vqm
	/*Quaternion pure_v = Quaternion(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 M;
	M[0][0] = 2; M[2][2] = 2;
	Quaternion q = Quaternion(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	Quaternion r = Quaternion(0.0f, glm::vec3(-1.0f, 2.0f, -1.0f));

	Quaternion res = M * r;

	Quaternion final = (q * (res * (q^-1))) + pure_v;*/

	glm::vec3 V(1.0f, 1.0f, 1.0f);
	Quaternion Q = Quaternion(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 M;
	M[0][0] = 2; M[2][2] = 2;
	glm::vec3 r(-1.0f, 2.0f, -1.0f);
	VQM vqm(V, Q, M);
	glm::vec3 Tr = vqm * r;



	int a = 0;
	a++;

	/*glm::vec3 V = glm::vec3(1.0f);
	Quaternion Q = Quaternion(1.0f, glm::vec3(1.0f, 0.0f,	 0.0f));
	glm::mat4 M;
	M[0][0] = 2.0; M[2][2] = 2.0;

 	VQM vqm = VQM(V, Q, M);
	glm::vec3 v(3.0f, 4.0f, 5.0f), res;

	res = vqm * v;*/

	for (size_t i = 2; i <= 6; ++i)
	{
		Matrix A = makeMatrix(i, i);
		//printMatrix(A);

		Matrix R = multiplyMatrices(A, A);
		printMatrix(R);

	}

	
	
	

	

	

	

	

	ReadHomeworkConfig();

	if (!Init())
		return -1;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//ndsShader->Init("Deferred_GBufferPass_Vertex.glsl", "NDCShaderFragment.glsl");
	shader->Init("DefaultVertex.glsl", "DefaultFragment.glsl");
	meshParticleShader->Init("MeshParticleVertex.glsl", "MeshParticleFragment.glsl");
	oceanShader->Init("OceanVertex.glsl", "OceanFragment.glsl");
	//particleShader->Init("ClothVertex.glsl", "ClothFragment.glsl");

	renderManager->AttachShaders(shader);
	renderManager->InitCameras();

	resourceManager->GenerateGameObjects();
	
	resourceManager->Start();

	//physics3DManager->Init();

	//resourceManager->SpawnNewGameObjectFromArchetypeForDeferredRendering("SphereLight");
	

	
	RunGame();

	Cleanup();

	return 0;
}

void RunGame()
{
	frameRateController->SetTargetFrameRate(60.0f);
	frameRateController->SetFrameAtLevelStart(
		frameRateController->GetFrameNumber());

	while (true)
	{
		frameRateController->FrameStart();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			uiManager->UIPRocessEvents(&event);
			if (event.type == SDL_QUIT)
				return;
			if (!TestColors(event))
				return;
			mouseManager->e = event;
		}

		mouseManager->UpdateMouseState();
		inputManager->UpdateKeyStates();
		physicsManager->Update();
		//physics3DManager->Update();
		uiManager->UINewFrame();

		

		RenderStuff();

		frameRateController->FrameEnd();

		//printf("FPS: %f \n Frame#: %i \n Last frame time: %i\n\n\n", frameRateController->GetFPS(), frameRateController->GetFrameNumber(), frameRateController->GetFrameTime());
	}
}

void RenderStuff()
{
	uiManager->TestImGui();
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	uiManager->UISceneClearColor();
	
	switch(homework)
	{
		case 1:
		{
			RunAnimationHomework1();
			break;
		}
	}

	uiManager->UIRender();
	if (frameRateController->GetFrameNumber() > 1)
		SDL_GL_SwapWindow(mainWindow);

	//buffer->save(true);
}


void RenderQuadAmbient()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // vertex
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1); // texcoord
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}

	

	

	int posUnit = renderManager->GetRender3D()->GetGBufferObject()->position;
	int normUnit = renderManager->GetRender3D()->GetGBufferObject()->normal;
	int ambiUnit = renderManager->GetRender3D()->GetGBufferObject()->ambient;
	int intsUnit = renderManager->GetRender3D()->GetGBufferObject()->intensity;

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLint pid = 0;
	GLint loc = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pid);

	//glActiveTexture(GL_TEXTURE0 + 0);
	//loc = glGetUniformLocation(pid, "gbPosition");
	//glUniform1i(loc, 0);
	//glBindTexture(GL_TEXTURE_2D, posUnit);
	loc = glGetUniformLocation(pid, "height");
	glUniform1i(loc, SCREEN_HEIGHT);
	loc = glGetUniformLocation(pid, "width");
	glUniform1i(loc, SCREEN_WIDTH);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, ambiUnit);
	loc = glGetUniformLocation(pid, "gbAmbient");
	glUniform1i(loc, 0);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);

}

void RenderLocalLights()
{
	int posUnit = renderManager->GetRender3D()->GetGBufferObject()->position;
	int normUnit = renderManager->GetRender3D()->GetGBufferObject()->normal;
	int ambiUnit = renderManager->GetRender3D()->GetGBufferObject()->ambient;
	int intsUnit = renderManager->GetRender3D()->GetGBufferObject()->intensity;

	GLint pid = 0;
	GLint loc = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pid);

	GLint is2D = 1;
	GLint _2DLoc = glGetUniformLocation(pid, "Lights");
	glUniform1i(_2DLoc, is2D);

	loc = glGetUniformLocation(pid, "height");
	glUniform1i(loc, SCREEN_HEIGHT);
	loc = glGetUniformLocation(pid, "width");
	glUniform1i(loc, SCREEN_WIDTH);

	glActiveTexture(GL_TEXTURE0 + 5);
	loc = glGetUniformLocation(pid, "gbPosition");
	glUniform1i(loc, 5);
	glBindTexture(GL_TEXTURE_2D, posUnit);

	glActiveTexture(GL_TEXTURE0 + 1);
	loc = glGetUniformLocation(pid, "gbAmbient");
	glUniform1i(loc, 1);
	glBindTexture(GL_TEXTURE_2D, ambiUnit);

	glActiveTexture(GL_TEXTURE0 + 2);
	loc = glGetUniformLocation(pid, "gbNormal");
	glUniform1i(loc, 2);
	glBindTexture(GL_TEXTURE_2D, normUnit);

	glActiveTexture(GL_TEXTURE0 + 3);
	loc = glGetUniformLocation(pid, "gbIntensity");
	glUniform1i(loc, 3);
	glBindTexture(GL_TEXTURE_2D, intsUnit);



	resourceManager->Update3DDeferredObjects();


}

void RenderQuadLight()
{

	//glBindVertexArray(quadLightVAO);quadVAO
	int posUnit = renderManager->GetRender3D()->GetGBufferObject()->position;
	int normUnit = renderManager->GetRender3D()->GetGBufferObject()->normal;
	int ambiUnit = renderManager->GetRender3D()->GetGBufferObject()->ambient;
	int intsUnit = renderManager->GetRender3D()->GetGBufferObject()->intensity;


	GLint pid = 0;
	GLint loc = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pid);

	loc = glGetUniformLocation(pid, "height");
	glUniform1i(loc, SCREEN_HEIGHT);
	loc = glGetUniformLocation(pid, "width");
	glUniform1i(loc, SCREEN_WIDTH);

	glActiveTexture(GL_TEXTURE0 + 0);
	loc = glGetUniformLocation(pid, "gbPosition");
	glUniform1i(loc, 0);
	glBindTexture(GL_TEXTURE_2D, posUnit);

	glActiveTexture(GL_TEXTURE0 + 1);
	loc = glGetUniformLocation(pid, "gbAmbient");
	glUniform1i(loc, 1);
	glBindTexture(GL_TEXTURE_2D, ambiUnit);

	glActiveTexture(GL_TEXTURE0 + 2);
	loc = glGetUniformLocation(pid, "gbNormal");
	glUniform1i(loc, 2);
	glBindTexture(GL_TEXTURE_2D, normUnit);

	glActiveTexture(GL_TEXTURE0 + 3);
	loc = glGetUniformLocation(pid, "gbIntensity");
	glUniform1i(loc, 3);
	glBindTexture(GL_TEXTURE_2D, intsUnit);

	renderManager->GetRender3D()->DeferredLightingPass();


	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);

}

void RenderQuad_Debug()
{
	if (quadDebugVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadDebugVAO);
		glGenBuffers(1, &quadDebugVBO);
		glBindVertexArray(quadDebugVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadDebugVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // vertex
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1); // texcoord
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadDebugVAO);

	int posUnit = renderManager->GetRender3D()->GetGBufferObject()->position;
	int normUnit = renderManager->GetRender3D()->GetGBufferObject()->normal;
	int ambiUnit = renderManager->GetRender3D()->GetGBufferObject()->ambient;
	int intsUnit = renderManager->GetRender3D()->GetGBufferObject()->intensity;


	if (inputManager->IsKeyPressed(SDL_SCANCODE_P))
	{
		input = 1;
	}
	if (inputManager->IsKeyPressed(SDL_SCANCODE_N))
	{
		input = 2;
	}
	if (inputManager->IsKeyPressed(SDL_SCANCODE_K))
	{
		input = 3;
	}
	if (inputManager->IsKeyPressed(SDL_SCANCODE_I))
	{
		input = 4;
	}


	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	switch (input)
	{
	case 1:
	{
		//simpleShader->ChooseTextureUnit(1, "gbPosition");
		glBindTexture(GL_TEXTURE_2D, posUnit);
		break;
	}
	case 2:
	{
		/*simpleShader->ChooseTextureUnit(2, "gbNormal*/
		glBindTexture(GL_TEXTURE_2D, normUnit);
		break;
	}
	case 3:
	{
		/*simpleShader->ChooseTextureUnit(3, "gbAmbient");*/
		glBindTexture(GL_TEXTURE_2D, ambiUnit);
		break;
	}
	case 4:
	{
		/*simpleShader->ChooseTextureUnit(4, "gbIntensity");*/
		glBindTexture(GL_TEXTURE_2D, intsUnit);
		break;
	}
	default:
	{
		/*simpleShader->ChooseTextureUnit(5, "gbPosition");*/
		glBindTexture(GL_TEXTURE_2D, posUnit);
		break;
	}

	}


	//glActiveTexture(0);
	//glBindTexture(GL_TEXTURE_2D, last_texture);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//glBindBuffer(GL_FRAMEBUFFER, 0);
	
	glBindVertexArray(0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}


void PrintSDL_GL_Attributes()
{
	int value = 0;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
	std::cout << "SDL_GL_CONTEXT_MAJOR_VERSION : " << value << std::endl;

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
	std::cout << "SDL_GL_CONTEXT_MINOR_VERSION: " << value << std::endl;
}

void Cleanup()
{
	resourceManager->DeleteGameObjects();

	delete eventManager;
	delete physicsManager;
	//delete physics3DManager;
	delete resourceManager;
	delete frameRateController;
	delete inputManager;
	delete renderManager;
	delete shader;
	delete meshParticleShader;
	delete oceanShader;
	//delete ndsShader;

	// Delete ImGui
	ImGui_ImplSdlGL3_Shutdown();

	// Delete our OpengL context
	SDL_GL_DeleteContext(mainContext);

	// Destroy our window
	SDL_DestroyWindow(mainWindow);

	// Shutdown SDL 2
	SDL_Quit();
}

bool TestColors(SDL_Event event)
{
	//while (SDL_PollEvent(&event))
	//{

	if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			return false;
		case SDLK_r:
			glClearColor(1.0, 0.0, 0.0, 1.0);
			break;
		case SDLK_g:
			glClearColor(0.0, 1.0, 0.0, 1.0);
			break;
		case SDLK_b:
			glClearColor(0.0, 0.0, 1.0, 1.0);
			break;
		case SDLK_q:
			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		default:
			break;
		}
	}
	//}
	return true;
}

bool Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Failed to init SDL\n";
		return false;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	mainWindow = SDL_CreateWindow(programName.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_OPENGL);


	if (!mainWindow)
	{
		std::cout << "Unable to create window\n";
		CheckSDLError(__LINE__);
		return false;
	}
	//SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	mainContext = SDL_GL_CreateContext(mainWindow);

	SetOpenGLAttributes();
	SDL_GL_SetSwapInterval(1);

	glewExperimental = GL_TRUE;
	glewInit();

	std::stringstream errorMessage;

	if (TTF_Init() != 0)
	{
		printf("Error initializing ttf");
		return false;
	}

	//TODO - add an exception controller
	shader = new Shader;
	meshParticleShader = new Shader;
	oceanShader = new Shader;

	renderManager = new RenderManager;
	mouseManager = new MouseManager;
	inputManager = new InputManager;
	frameRateController = new FrameRateController;
	resourceManager = new ResourceManager;
	physicsManager = new PhysicsManager;
	//physics3DManager = new Pyshics3DManager;
	eventManager = new EventManager;

	// setup imgui
	uiManager->UIInit(mainWindow);

	// video buffer
	buffer = new VideoBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);

	return true;

}

void CheckSDLError(int line)
{
	std::string error = SDL_GetError();

	if (error != "")
	{
		std::cout << "SDL Error : " << error.c_str() << std::endl;

		if (line != -1)
			std::cout << "\nLine : " << line << std::endl;

		SDL_ClearError();
	}
}

bool SetOpenGLAttributes()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	return true;
}

void ReadHomeworkConfig()
{

	uiManager = new UIManager;

	std::vector<std::string> sceneGameObjects;
	std::string fileContent = readconfigfile("HomeworkConfig.txt");
	std::istringstream contentStream(fileContent.c_str());
	std::string word;
	std::string line;

	while (contentStream >> word)
	{
		if (word == "#")
		{
			std::getline(contentStream, line);
			continue;
		}

		if (word == "hw")
		{
			std::getline(contentStream, line);
			line.erase(
				std::remove_if(line.begin(),
					line.end(), isspace),
				line.end()
			);
			if (line == "graphics")
			{
				uiManager->HwName = HW_GRAPHICS;
				homework = 0;
			}
				
			if (line == "animation")
			{
				uiManager->HwName = HW_ANIMATION;
				homework = 1;
			}
				

			continue;
		}
	}

}


std::string readconfigfile(const char* fileName)
{
	std::string line;
	std::stringstream fileString;
	std::ifstream file(fileName);
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			fileString << line << "\n";
		}
		file.close();
	}
	else
	{
		printf("Unable to open game object file.");
	}
	return fileString.str();
}

void RunAnimationHomework1()
{
	renderManager->Clear();
	shader->UseProgram();
	renderManager->Update3DCamera();
	renderManager->GetRender3D()->RegularLightingUpdate();
	resourceManager->Update3DGameObjects();

	renderManager->Use2D();
	resourceManager->Update2DGameObjects();
}