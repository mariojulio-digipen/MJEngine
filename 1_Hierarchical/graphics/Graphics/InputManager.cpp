#include "InputManager.h"
#include <string.h>
#include <iostream>


InputManager::InputManager()
{
	//int numkeys;
	currentKeyStates = SDL_GetKeyboardState(NULL);


	for (size_t i = 0; i < 512; i++)
	{
		isPressed[i] = 0;
		isReleased[i] = 1;
		isPressedInPreviousFrame[i] = 0;
		isReleasedInPreviousFrame[i] = 0;
	}
}

InputManager::~InputManager()
{
}

void InputManager::UpdateKeyStates()
{
	for (size_t i = 0; i < 512; i++)
	{
		isPressedInPreviousFrame[i] = isPressed[i];
		isReleasedInPreviousFrame[i] = isReleased[i];

		if (currentKeyStates[i])
		{
			isPressed[i] = 1;
			isReleased[i] = 0;
		}
		else
		{
			isPressed[i] = 0;
			isReleased[i] = 1;
		}
	}
}

Uint8 InputManager::IsKeyDown(int key)
{
	return currentKeyStates[key];
}

Uint8 InputManager::IsKeyUp(int key)
{
	return !currentKeyStates[key];
}

Uint8 InputManager::IsKeyPressed(int key)
{
	return isPressed[key] && !isPressedInPreviousFrame[key];
}

Uint8 InputManager::IsKeyReleased(int key)
{
	return isPressedInPreviousFrame[key] && !isPressed[key];
}