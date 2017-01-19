#pragma once

#include "ImGui\imgui.h"
#include "MJImGui.h"
#include "SDL.h"

#include <list>
#include <vector>

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

