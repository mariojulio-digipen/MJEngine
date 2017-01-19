#include "Camera3D.h"
#include "Shader.h"
#include "Quaternion.h"
#include "VQS.h"
#include "UIManager.h"

#include <gtc\matrix_transform.hpp>
#include <gtx\transform.hpp>
#include <gtx\euler_angles.hpp>
#include <stdio.h>
#include <glew.h>
#include <iostream>

extern Shader* shader;
extern UIManager* uiManager;


void Camera3D::orbit(glm::vec3 target)
{
	glm::vec3 Up_N = glm::normalize(Up);
	Right = glm::vec3(glm::cross(target, Up_N));
	glm::vec3 Right_N = glm::normalize(Right);


	glm::vec3 camTargetDirection = Position - target;

	// with VQS (VQ actually)
	glm::vec3 angles = glm::vec3(glm::radians(-moveDelta.y), glm::radians(-moveDelta.x), 0.0f);
	glm::vec3 translator = target;
	Quaternion rotator = Quaternion::QuatFromAngles(angles);
	VQS VQS_Transform = VQS(translator, rotator, 1.0f);
	glm::vec3 newpos = VQS_Transform * camTargetDirection;
	Position = glm::vec3(newpos);

	View = glm::lookAt(Position, target, Up);
	MouseMove->OldPosition = MouseMove->Position;

	std::cout << "Position (" << Position.x << ", " 
		<< Position.y << ", " << Position.z << ")" << std::endl;
	std::cout << "Pitch: " << angles.x << std::endl;

}

void Camera3D::buildView()
{
	angle = angle + moveDelta;
	Right = glm::vec3(glm::cross(ViewDirection, Up));
	glm::mat4 rot = glm::rotate(-moveDelta.x, Up) * glm::rotate(-moveDelta.y, Right); 
	ViewDirection = glm::mat3(rot) * ViewDirection;

	View = glm::lookAt(Position, Position + ViewDirection, Up);
	MouseMove->OldPosition = MouseMove->Position;
}

void Camera3D::buildProjection()
{
	float ratio = Ratio;
	float fov = glm::radians(FovAngleDegrees);
	Projection = glm::perspective(fov, ratio, near, far);
}

void Camera3D::move()
{
	/*std::cout << "Position (" << Position.x << ", "
		<< Position.y << ", " << Position.z << ")" << std::endl;
	std::cout << "view direction: (" << ViewDirection.x << ", " << ViewDirection.y << ", " << ViewDirection.z << ")" << std::endl;
*/
	mouseEventsUpdate();
	keyboardStateUpdate();
}

void Camera3D::Update()
{
	move();
	//orbit(ViewDirection);
	buildView();
	buildProjection();

	glm::vec3 cameraPos = this->Position;
	GLint cameraPosLocation = glGetUniformLocation(shader->GetProgramID(), "CameraPos");
	glUniform3fv(cameraPosLocation, 1, &cameraPos[0]);
}

void Camera3D::keyboardStateUpdate()
{
	if (InputMove->IsKeyDown(SDL_SCANCODE_W))
	{
		Position += ViewDirection*uiManager->CameraSpeed;
		//printf("z cam pos: %f\n", Position.z);
	}
	if (InputMove->IsKeyDown(SDL_SCANCODE_S))
	{
		Position -= ViewDirection*uiManager->CameraSpeed;
		//printf("z cam pos: %f\n", Position.z);
	}


	if (InputMove->IsKeyDown(SDL_SCANCODE_A))
	{
		Position -= Right*uiManager->CameraSpeed;
	}
	if (InputMove->IsKeyDown(SDL_SCANCODE_D))
	{
		Position += Right*uiManager->CameraSpeed;
	}
}

void Camera3D::mouseEventsUpdate()
{
	Uint32 left, middle, right;
	left = MouseMove->IsLeftPressed();
	middle = MouseMove->IsMiddlePressed();
	right = MouseMove->IsRightPressed();

	if (!middle)
		rayCasted = false;

	

	if (rayCasted)
	{
		stopCasting = true;
		return;
	}
		

	if (right)
	{
		//moveDelta += MouseMove->Position - MouseMove->OldPosition;
		moveDelta = MouseMove->Position - MouseMove->OldPosition;
		moveDelta *= 0.005f;
	}
	else if (middle)
	{
		/*translateDelta = MouseMove->Position - MouseMove->OldPosition;
		if(translateDelta.x < 0)
			Position += Right*0.09f;
		if (translateDelta.x > 0)
			Position -= Right*0.09f;
		if (translateDelta.y < 0)
			Position -= Up*0.09f;
		if (translateDelta.y > 0)
			Position += Up*0.09f;*/
		Ray ray(Projection, View);
		ray.Cast();
		CamRayDirection = ray.GetDirection();
		//std::cout << "Acabas de castear un rayo";
		rayCasted = true;
	}
	else if (left)
	{
		//ViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		// create a condition to raycast only
		// if user wants it
		// uiManager can be used for that purpose

		// for now, it's always raycasting
		
	}
	else
	{
		rayCasted = false;
		stopCasting = false;
		moveDelta = glm::vec2(0.0f, 0.0f);
	}
		

	//if (left || middle || right)
		//printf("Mouse Position: (%f, %f)\n", MouseMove->Position.x, MouseMove->Position.y);
}


// Camera inner class: Ray (Maybe will be used by other classes in the future)

extern MouseManager* mouseManager;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

Ray::Ray()
{
}

Ray::~Ray()
{
}

void Ray::Cast()
{
	float mouseX, mouseY;

	// set viewport coords from mouse position in screen
	mouseX = mouseManager->Position.x;
	mouseY = mouseManager->Position.y;
	this->viewportCoords = glm::vec2(mouseX, mouseY);

	// set normalized device coordinates
	float x, y, z;
	x = (2.0f * mouseX) / SCREEN_WIDTH - 1.0f;
	y = 1.0f - (2.0f * mouseY) / SCREEN_HEIGHT;
	z = -1.0f;
	this->ndcCoords = glm::vec3(x, y, z);

	// get the homogenous clip coordinate
	glm::vec4 rayClip = glm::vec4(
		this->ndcCoords.x, this->ndcCoords.y, this->ndcCoords.z, 1.0f);

	// camera coordinates (un-project the x,y part)
	glm::vec4 rayEye = glm::inverse(this->projection) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

	// un-view the rayEye
	glm::vec4 tmp = glm::inverse(this->view) * rayEye;
	glm::vec3 dir = glm::vec3(tmp);
	dir = glm::normalize(dir);

	this->direction = dir;
	
}
