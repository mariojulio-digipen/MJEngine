#pragma once

#include <SDL.h>

class InputManager
{
public:
	InputManager();
	~InputManager();
	void UpdateKeyStates();
	Uint8 IsKeyDown(int key);
	Uint8 IsKeyUp(int key);
	Uint8 IsKeyPressed(int key);
	Uint8 IsKeyReleased(int key);

private:
	SDL_Event e;
	const Uint8* currentKeyStates;

	Uint8 isPressed[512];
	Uint8 isReleased[512];

	Uint8 isPressedInPreviousFrame[512];
	Uint8 isReleasedInPreviousFrame[512];

	int length;
};
