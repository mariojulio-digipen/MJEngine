#include "FrameRateController.h"

#include <iostream>

#define TICKS_PER_SECOND 1000

FrameRateController::FrameRateController()
{
	frameNumber = 0;
	ticksAtFrameStart = 0;
	ticksAtFrameEnd = 0;
	lastRenderTime = 0;
	lastFrameTime = 0;
	targetFrameTime = (Uint32)(TICKS_PER_SECOND / 60.0f);
}

FrameRateController::~FrameRateController()
{

}

void FrameRateController::SetTargetFrameRate(const float frameRate)
{
	targetFrameTime = (Uint32)(TICKS_PER_SECOND / frameRate);
}

void FrameRateController::FrameStart()
{
	
	//DeltaTime = 1.0f;
	ticksAtFrameStart = SDL_GetTicks();
}

void FrameRateController::FrameEnd()
{
	lastRenderTime = SDL_GetTicks() - ticksAtFrameStart;

	ticksAtFrameEnd = SDL_GetTicks();
	Uint32 delta = ticksAtFrameEnd - ticksAtFrameStart;

	while (ticksAtFrameEnd - ticksAtFrameStart < targetFrameTime)
		ticksAtFrameEnd = SDL_GetTicks();
	++frameNumber;

	lastFrameTime = ticksAtFrameEnd - ticksAtFrameStart;
	DeltaTime = static_cast<float>(lastFrameTime);
	//frameTime = tickEnd - tickStart;


	/*lastRenderTime = SDL_GetTicks() - ticksAtFrameStart;
	while ((ticksAtFrameEnd = SDL_GetTicks()) - ticksAtFrameStart < targetFrameTime);
	++frameNumber;*/
}