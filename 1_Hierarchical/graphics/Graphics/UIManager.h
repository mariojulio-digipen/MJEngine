#pragma once

#include "ImGui\imgui.h"
#include "MJImGui.h"
#include "SDL.h"

#include <list>
#include <vector>
#include <glm.hpp>

enum HomeWork
{
	HW_GRAPHICS,
	HW_ANIMATION,
	HW_MAX
};


class UIManager
{
private:

	

	SDL_Window* mainWindow;
	bool active;

	std::list<std::string> debugMessages;

	bool show_test_window = false;
	bool show_another_window = false;
	
	int deferred_debug = 0;


	int shading = 0;


	void UIShowSampleWindow();
	void UISimpleWindow();
	void UITestWindow();

public:

	int LightsNumber;
	std::vector<ImVec4> LightsColors;

	std::vector<ImVec4*> ObjectsPosition;
	std::vector<ImVec4*> ObjectsRotation;
	std::vector<ImVec4*> ObjectsScale;
	std::vector<std::string> ObjectsName;
	std::vector<ImVec4*> ObjectsLightColor;
	std::vector<bool*> selected; // to show a bone is selected
	bool Orbit;
	float CameraSpeed;
	int AnimationSpeed = 4;
	float SplineDefinition = 0.001f;
	bool SplineReset = false;
	bool SplineHidden = false;
	float PathPeace = 1.0f;
	bool Playing = true;
	bool RenderDebugMode = false;
	float AnimPerSec = 0.0f;

	float CCDRotate = false;
	float CCDRotateContinuous = true;
	float CCDCosine = 0.0f;
	float CCDAngle = 0.0f;
	bool CCDCloseEnough = false;
	float CCDDesiredDistance = 0.0f;

	int GetShadingMode();
	int IsDeferredDebugMode();

	float DumpFactor = 0.0f; 
	float Gravity = -9.8f;
	float Scale = 0.4f;

	bool Stick1Released = true;
	bool Stick2Released = true;
	bool Stick3Released = true;
	bool Stick4Released = true;

	float Stick1LocationX;
	float Stick1LocationY;
	float Stick1LocationZ;

	// particles
	bool vortex = true;
	bool spiral = false;

	// Vortex
	bool VortexActive = false;
	float VortexTightnes = 1.5f;
	float VortexRotationRate = 10.0f;

	// Spiral
	bool SpiralActive = false;
	glm::vec3 SpiralSpawnLocation = glm::vec3(1.0f);
	glm::vec3 SpiralSpawnVelocity = glm::vec3(5.7f, 0.95f, 1.0f);
	int SpiralMaxParticles;// = 100;
	int SpiralLifeSpan;// = 20;
	float SpiralSigma;// = 1.0f;
	bool CoolEffect = false;
	float DumpingFactor = 0.03f;
	bool AccelIncrease = false;

	// Orbit
	float BHGravity = 15.0f;
	float BHMass = 25.0f;


	// Ocean
	float OceanGridLength = 64;
	float OceanTimeStep = 0.5f;
	float HeigthControl = 0.05f;

	UIManager();
	~UIManager();

	

	void UIInit(SDL_Window* mainwindow);
	void UINewFrame();
	void UIPRocessEvents(SDL_Event* event);
	void UIRender();
	void UIActivateSwitch();

	void TestImGui();
	

	void UISceneClearColor();

	void PushDebugMessage(std::string msg);
	ImVec4 clear_color = ImColor(114, 144, 154);

	HomeWork HwName;

};

