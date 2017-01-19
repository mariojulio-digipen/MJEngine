#include "Camera2D.h"
#include "SDL.h"
#include <gtc\matrix_transform.hpp>
#include <gtx\transform.hpp>

#include <iostream>


extern SDL_Window* mainWindow;


Camera2D::Camera2D() : near(0.1f), far(1000.0f)
{
	SDL_GetWindowSize(mainWindow, &WinWidth, &WinHeight);
}

void Camera2D::buildView()
{
	View = glm::lookAt(Position, Position + ViewDirection, Up);
}

void Camera2D::buildProjection()
{
	float bottom, top, left, right;
	SDL_GetWindowSize(mainWindow, &WinWidth, &WinHeight);
	bottom = -WinHeight*0.5f;
	top = WinHeight*0.5f;
	left = -WinWidth*0.5f;
	right = WinWidth*0.5f;
	Projection = glm::ortho(left, right, bottom, top, near, far);
}




void Camera2D::Update()
{
	buildView();
	buildProjection();
	printf("");
}