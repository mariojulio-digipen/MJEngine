#pragma once
#include <SDL.h>
#include <glm.hpp>

class MouseManager
{
private:
	void mouseCoordinates();

public:
	SDL_Event e;
	glm::vec2 Position;
	glm::vec2 OldPosition;

	MouseManager();
	~MouseManager();

	void UpdateMouseState();
	Uint32 IsLeftPressed();
	Uint32 IsMiddlePressed();
	Uint32 IsRightPressed();
	

};

