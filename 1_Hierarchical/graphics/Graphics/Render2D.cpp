#include "Render2D.h"
#include "Shader.h"
#include "GameObject.h"
#include "SDL.h"
#include "UIQuad.h"

#include "RenderManager.h"

extern RenderManager* renderManager;

extern SDL_Window* mainWindow;

void Render2D::InitCamera()
{
	CurrentCamera->ViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	CurrentCamera->Position = glm::vec3(0.0f, 0.0f, 3.0f);
	CurrentCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Render2D::UpdateCamera(int* width, int* height)
{
	CurrentCamera->WinWidth = (*width);
	CurrentCamera->WinHeight = (*height);
	CurrentCamera->Update();
}

void Render2D::Use()
{

	int w, h;
	SDL_GetWindowSize(mainWindow, &w, &h);
	
	defaultShader->Enable2D();
	UpdateCamera(&w, &h);
}

//void Render2D::InitQuads()
//{
//	quad->Init();
//
//}

//void Render2D::UpdateQuads()
//{
//	defaultShader->Enable2D();
//	quad->Update();
//	defaultShader->Disable2D();
//
//}

//bool Render2D::SendDataToGPU()
//{
//	quad->SendDataToGPU();
//	if (!defaultShader->Init())
//		return false;
//
//
//
//	return true;
//}

//bool Render2D::CreateBuffers()
//{
//	quad = new UIQuad(true, false, defaultShader, CurrentCamera);
//	quad->CreateBuffers();
//
//
//
//	return true;
//}