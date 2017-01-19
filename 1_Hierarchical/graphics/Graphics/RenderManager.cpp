#include "RenderManager.h"
#include "Render2D.h"
#include "Render3D.h"
#include "Shader.h"



RenderManager::RenderManager() : render2D(new Render2D), render3D(new Render3D)
{
}


RenderManager::~RenderManager()
{
	if(render2D != NULL)
		delete render2D;
	if(render3D != NULL)
		delete render3D;
}

void RenderManager::InitCameras() const
{
	render3D->InitCamera();
	render2D->InitCamera();
}

void RenderManager::AttachShaders(Shader* shader) const
{
	render3D->AttachShader(shader);
	render2D->AttachShader(shader);
}

void RenderManager::Update3DCamera() const
{
	render3D->Update();
}

//void RenderManager::Use3DGBuffer() const
//{
//
//}

void RenderManager::Use2D() const
{
	render2D->Use();
}

void RenderManager::Clear() const
{
	//glDepthFunc(GL_LESS);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void RenderManager::EnableUseOfTextures() const
{
	glEnable(GL_TEXTURE_2D);
}

void RenderManager::EnableCullFacing(GLenum mode) const
{
	glEnable(GL_CULL_FACE);
	glCullFace(mode);
}

void RenderManager::DisableCullFacing() const
{
	glDisable(GL_CULL_FACE);
}

//void RenderManager::RegularLightingPass()
//{
//	render3D->RegularLightingPass();
//}

//void RenderManager::GBufferPass()
//{
//	render3D
//}