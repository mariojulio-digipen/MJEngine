#pragma once
#include <SDL.h>

// Controls frame rate by calling SDL functions.
class FrameRateController
{
public:
	FrameRateController();
	~FrameRateController();


	void SetTargetFrameRate(const float frameRate);
	void SetFrameAtLevelStart(const Uint32 time) { frameAtLevelStart = time; };


	void FrameStart();
	void FrameEnd();


	Uint32 GetFrameTime() const { return lastFrameTime; }
	float GetFPS() const { return 1000.0f / lastFrameTime; }


	Uint32 GetFrameNumber() const { return frameNumber; }


	Uint32 GetTick() const { return ticksAtFrameEnd; }


	Uint32 GetRenderTime() const { return lastRenderTime; }

	Uint32 GetFrameAtLevelStart() const { return frameAtLevelStart; }

	double GetTime() const { return static_cast<double>(targetFrameTime); }

	int GetSeconds() const { return SDL_GetTicks() / 1000; }

	float DeltaTime;

private:
	Uint32 frameNumber;
	Uint32 frameAtLevelStart;
	Uint32 ticksAtFrameStart;
	Uint32 ticksAtFrameEnd;
	Uint32 lastFrameTime;
	Uint32 lastRenderTime;
	Uint32 targetFrameTime;
};