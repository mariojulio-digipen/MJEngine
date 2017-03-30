#include "Lighter.h"
#include "Camera3D.h"
#include "Render3D.h"
#include "RenderManager.h"

extern RenderManager* renderManager;

Lighter::Lighter()
{
}


Lighter::~Lighter()
{
}


void Lighter::ProcessLights(int shaderID, LightPointComponent* light)
{
	Camera3D* currentCamera = renderManager->GetRender3D()->CurrentCamera;
	int shaderUsed = shaderID;//particleSystem->GetShaderUsed();

	GLuint posLoc = glGetUniformLocation(shaderUsed, "LightPositionIn");
	glUniform3fv(posLoc, 1, &light->GetLightPosition()[0]);


	GLuint ambientLoc = glGetUniformLocation(shaderUsed, "LightAmbientIn");
	glUniform3fv(ambientLoc, 1, &light->GetLightAmbient()[0]);


	GLuint intensityLoc = glGetUniformLocation(shaderUsed, "LightIntensityIn");
	glUniform3fv(intensityLoc, 1, &light->GetLightIntensity()[0]);


	GLuint colorLoc = glGetUniformLocation(shaderUsed, "LightColorIn");
	glUniform3fv(colorLoc, 1, &light->GetLightColor()[0]);


	GLint proj = glGetUniformLocation(shaderUsed, "Projection");
	glUniformMatrix4fv(proj, 1, GL_FALSE, &currentCamera->Projection[0][0]);

	GLint view = glGetUniformLocation(shaderUsed, "View");
	glUniformMatrix4fv(view, 1, GL_FALSE, &currentCamera->View[0][0]);

	glm::vec3 cameraPos = currentCamera->Position;
	GLint cameraPosLocation = glGetUniformLocation(shaderUsed, "CameraPos");
	glUniform3fv(cameraPosLocation, 1, &cameraPos[0]);
}