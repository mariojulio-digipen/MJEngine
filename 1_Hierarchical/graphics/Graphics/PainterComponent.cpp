#include "PainterComponent.h"
#include "RenderManager.h"
#include "Render2D.h"
#include "GameObject.h"
#include "GLObject.h"
#include "Shader.h"
#include "TransformComponent.h"
#include "UIManager.h"

#include "ResourceManager.h"

#define TOOLKIT_CHECK_GL_ERROR {GLenum error; if ((error = glGetError()) != GL_NO_ERROR) {printf("%s line %d: ", __FILE__, __LINE__); PainterComponent::PrintGLError(error);}}



extern ResourceManager* resourceManager;
extern Shader* shader;
extern UIManager* uiManager;

PainterComponent::PainterComponent() : visible(true)
{

}


PainterComponent::~PainterComponent()
{
}

bool PainterComponent::Init()
{
	glObject = GetOwner()->GetGLObject();
	trans = GetOwner()->FindComponent<TransformComponent>();
	
	
	return true;
}

void PainterComponent::Update()
{

	glObject->DebugMode = uiManager->RenderDebugMode;

	if (GetOwner()->GetParent())
	{
		parentTrans = GetOwner()->GetParent()->FindComponent<TransformComponent>();
	}

	if (visible)
	{
		//if (GetOwner()->GetDefaultShader()->GetRenderType() == R2D)
		if (GetOwner()->GetGLObject()->Type == R2D)
		{
			glDisable(GL_DEPTH_TEST);
			glObject->Update();
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			if(GetOwner()->IsDeferred)
				glDisable(GL_DEPTH_TEST);
			else
				glEnable(GL_DEPTH_TEST);

			
			if (GetOwner()->IsDeferred)
			{
				TransformComponent* trans = GetOwner()->FindComponent<TransformComponent>();

				GLint pid = 0;
				glGetIntegerv(GL_CURRENT_PROGRAM, &pid);

				GLint loc = 0;
				loc = glGetUniformLocation(pid, "ObjectScale");
				glUniform1f(loc, trans->GetScale().x);
			}

			glObject->Update();
			

			if (GetOwner()->IsDeferred)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
			//glDisable(GL_DEPTH_TEST);
		}
	}
	//TOOLKIT_CHECK_GL_ERROR
}

void PainterComponent::Destroy()
{

}

void PainterComponent::HandleEvent(const Event * const evt)
{

}
