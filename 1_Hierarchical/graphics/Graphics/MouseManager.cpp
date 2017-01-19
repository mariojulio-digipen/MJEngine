#include "MouseManager.h"

#include <stdio.h>

MouseManager::MouseManager() :
	Position(glm::vec2(0.0f, 0.0f)),
	OldPosition(glm::vec2(0.0f, 0.0f))
{
}


MouseManager::~MouseManager()
{
}

void MouseManager::UpdateMouseState()
{
	mouseCoordinates();

}


Uint32 MouseManager::IsLeftPressed()
{
	return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);
}

Uint32 MouseManager::IsMiddlePressed()
{
	return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE);
}

Uint32 MouseManager::IsRightPressed()
{
	return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT);
}

void MouseManager::mouseCoordinates()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	Position = glm::vec2(x, y);
}

