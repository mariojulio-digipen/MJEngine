#include "UIManager.h"
#include "SDL_syswm.h"
#include "glew.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "GLObject.h"
#include "FrameRateController.h"
#include <iostream>
#include <sstream>

extern InputManager* inputManager;
extern ResourceManager* resourceManager;
extern FrameRateController* frameRateController;

UIManager::UIManager() : active(true), LightsNumber(0)
{
	SpiralMaxParticles = 100;
	SpiralLifeSpan = 20;
	SpiralSigma = 1.0f;
}


UIManager::~UIManager()
{
}


void UIManager::UIInit(SDL_Window* mainWindow)
{
	ImGui_ImplSdlGL3_Init();
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(mainWindow, &wmInfo);

	ImGuiIO& io = ImGui::GetIO();
	io.ImeWindowHandle = wmInfo.info.win.window;

	this->mainWindow = mainWindow;
}

void UIManager::UINewFrame()
{
	ImGui_ImplSdlGL3_NewFrame(mainWindow);
	UIActivateSwitch();
}

void UIManager::UIPRocessEvents(SDL_Event* event)
{
	ImGui_ImplSdlGL3_ProcessEvent(event);
}

// 1. Show a simple window
// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
void UIManager::UIShowSampleWindow()
{
	static float f = 0.0f;
	
	//ImGui::Text("Active: %d", active);
	//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

	if (HwName == HW_GRAPHICS)
	{
		ImGui::Text("*** DEFERRED SHADING PANEL ***");


		ImGui::RadioButton("Release Mode", &deferred_debug, 0);
		ImGui::RadioButton("Debug Mode", &deferred_debug, 1);
		ImGui::Text("     - Press P to see Position");
		ImGui::Text("     - Press N to see Normal");
		ImGui::Text("     - Press K to see Diffuse");
		ImGui::Text("     - Press I to see Specular");

		/*ImGui::Text("Lights Colors");
		ImGui::Text("Press + to add more test lights");
		size_t n = static_cast<size_t>(LightsNumber);
		if (LightsColors.size() > 0)
		{
			for (size_t i = 1; i < n; i++)
			{
				std::stringstream conc;
				conc << "Light #" << i;
				std::string str = conc.str();
				ImGui::ColorEdit3(str.c_str(), (float*)&LightsColors[i - 1]);
			}
		}*/
	}

	// Camera Controls
	//float fps = frameRateController->GetFPS(); 
	//std::string _fps= std::to_string(fps);
	//ImGui::Text("Avg FPS: ----->"); ImGui::SameLine();
	//ImGui::Text(_fps.c_str());

	//// check time
	//float time = static_cast<float>(frameRateController->GetTick() /
	//	frameRateController->GetFrameNumber());
	//std::string t = std::to_string(time);
	//ImGui::Text("Time (t): ----->"); ImGui::SameLine();
	//ImGui::Text(t.c_str());

	//// check my time
	//float tt = static_cast<float>(frameRateController->GetTime());
	//std::string ttt = std::to_string(tt);
	//ImGui::Text("My Time (t): ----->"); ImGui::SameLine();
	//ImGui::Text(ttt.c_str());


	ImGui::Text("CS562 - ANIMATION");
	ImGui::Checkbox("Debug Mode", &RenderDebugMode);
	ImGui::Text(" ");

	/*ImGui::Text("--------------------");
	ImGui::Text("Debug CCD");
	if (ImGui::Button("Next"))
		CCDRotate = true;
	ImGui::SameLine();
	if (ImGui::Button("CCD Play"))
		CCDRotateContinuous = true;
	ImGui::SameLine();
	if (ImGui::Button("CCD Stop"))
		CCDRotateContinuous = false;*/

	/*std::string ccd_cosine = "Cosine: " + std::to_string(CCDCosine);
	std::string ccd_angle = "Angle: " + std::to_string(CCDAngle);
	std::string ccd_desiredDistance = "Distance: " + std::to_string(CCDDesiredDistance);
	ImGui::Text(ccd_cosine.c_str());
	ImGui::Text(ccd_angle.c_str());
	ImGui::Text(ccd_desiredDistance.c_str());*/

	/*std::string ccs_closeEnough = "Found Target: " + std::to_string(CCDCloseEnough);
	ImGui::Text(ccs_closeEnough.c_str());*/

	/*std::string anim_per_sec = std::to_string(AnimPerSec);
	ImGui::Text("Animation cycles per second: ----->"); ImGui::SameLine();
	ImGui::Text(anim_per_sec.c_str());
	ImGui::Text(" ");*/


	/*ImGui::Text("--------------------");
	ImGui::Text("Cloth Simulation");
	ImGui::Checkbox("Release stick 1", &Stick1Released);
	ImGui::Checkbox("Release stick 2", &Stick2Released);
	ImGui::Checkbox("Release stick 3", &Stick3Released);
	ImGui::Checkbox("Release stick 4", &Stick4Released);*/
	/*ImGui::Text("-------------------------------------------------------");
	ImGui::Text("X: "); ImGui::SameLine();
	ImGui::SliderFloat("Position_Stick1_X", (float*)&Stick1LocationX, -20.0f, 20.0f);
	ImGui::Text("Y: "); ImGui::SameLine();
	ImGui::SliderFloat("Position_Stick1_Y", (float*)&Stick1LocationY, 0.5f, 20.0f);
	ImGui::Text("Z: "); ImGui::SameLine();
	ImGui::SliderFloat("Position_Stick1_Z", (float*)&Stick1LocationZ, -20.0f, 20.0f);*/

	/*ImGui::Text("--------------------");
	ImGui::Text("Dumping");
	ImGui::Text("--------------------");
	ImGui::SliderFloat("Dumping", (float*)&DumpFactor, 0.0f, 1.0f);*/
	/*ImGui::Text("Gravity");
	ImGui::Text("--------------------");
	ImGui::SliderFloat("Gravity", (float*)&Gravity, -19.6f, 0.0f);
	ImGui::Text("--------------------");

	ImGui::Text("Cloth Scale");
	ImGui::Text("--------------------");
	ImGui::SliderFloat("Scale", (float*)&Scale, 0.1f, 4.0f);
	ImGui::Text("--------------------");*/


	ImGui::Text("--------------------");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text(" ");
	ImGui::Text("Press 2 to hide this window or 1 to restore it"); 
	ImGui::Text("Expand this window by dragging");
	ImGui::Text("from the bottom-right corner");
	ImGui::Text(" ");

	ImGui::Text("=================");
	ImGui::Text("|Camera Controls|");
	ImGui::Text("=================");
	ImGui::SliderFloat("Camera Speed", (float*)&CameraSpeed, 0.01f, 1.00f);
	ImGui::Text("");
	ImGui::Text("o Right-click and drag change the view direction of the camera");
	ImGui::Text("o Press WASD to move the camera around the world");
	/*ImGui::Text("o W to move forward without loosing the target point");
	ImGui::Text("o S to move backwards without loosing the target point");*/
	ImGui::Text(" ");
	ImGui::Text(" ");
	
	ImGui::Text("==================");
	ImGui::Text("|Ocean Controls|");
	ImGui::Text("==================");





	//ImGui::Checkbox("Spiral System", &SpiralActive);
	//ImGui::Checkbox("Vortex", &VortexActive);
	ImGui::SliderFloat("Length: ", (float*)&OceanGridLength, 1.0, 64.0f);
	ImGui::SliderFloat("Time Step: ", (float*)&OceanTimeStep, 0.0f, 10.0f);
	ImGui::SliderFloat("Height: ", (float*)&HeigthControl, 0.0f, 0.05f);


	//if (VortexActive)
	//{
	//	ImGui::Text("==================");
	//	ImGui::Text("|Vortex Controls|");
	//	ImGui::Text("==================");
	//	ImGui::SliderFloat("Tightness: ", (float*)&VortexTightnes, 1.0f, 2.0f);
	//	ImGui::SliderFloat("V. Rotation Rate: ", (float*)&VortexRotationRate, 1.0f, 10.0f);
	//}

	//if (SpiralActive)
	//{
	//	ImGui::Text("==================");
	//	ImGui::Text("|Spiral Controls|");
	//	ImGui::Text("==================");
	//	//ImGui::SliderInt("Number of Particles: ", (int*)&SpiralMaxParticles, 100, 1000);
	//	ImGui::Checkbox("Cool Predefined Effect", &CoolEffect);
	//	ImGui::Checkbox("Increase Acceleration", &AccelIncrease);
	//	ImGui::SliderInt("LifeSpan of Particles: ", (int*)&SpiralLifeSpan, 1, 30);
	//	ImGui::SliderFloat("S. Rotation Rate: ", (float*)&SpiralSigma, 1.0f, 10.0f);
	//	ImGui::SliderFloat3("Velocity", (float*)&SpiralSpawnVelocity[0], -1.0f, 1.0f);
	//	ImGui::SliderFloat("Black Hole Gravity: ", (float*)&BHGravity, 0.0f, 30.0f);
	//	ImGui::SliderFloat("Black Hole Mass: ", (float*)&BHMass, 1.0f, 50.0f);
	//	ImGui::SliderFloat("Dumping Factor: ", (float*)&DumpingFactor, 0.001f, 0.01f);
	//}


	
	/*ImGui::Text("o Press + button to speed up");
	ImGui::Text("o Press - button to speed down");
	ImGui::Text("o You can also pause/play the skeleton animation with pause/play button");
	ImGui::Text("  Or with the space bar");
	ImGui::Text("--------------------");*/

	/*ImGui::Text("ANIMATION SPEED -> "); ImGui::SameLine();
	if (ImGui::SmallButton("+") || inputManager->IsKeyPressed(SDL_SCANCODE_KP_PLUS))
		AnimationSpeed++;
	ImGui::SameLine();
	std::string speed = std::to_string(AnimationSpeed);
	ImGui::Text(speed.c_str()); ImGui::SameLine();
	if (ImGui::SmallButton("-") || inputManager->IsKeyPressed(SDL_SCANCODE_KP_MINUS))
		AnimationSpeed--;
	if (AnimationSpeed <= 0)
		AnimationSpeed = 1;*/

	/*ImGui::Text("ANIMATION PAUSE/PLAY -> "); ImGui::SameLine();
	if (Playing)
	{
		if (ImGui::SmallButton("Pause") || inputManager->IsKeyPressed(SDL_SCANCODE_SPACE))
			Playing = false;
	}else
	{
		if (ImGui::SmallButton("Play") || inputManager->IsKeyPressed(SDL_SCANCODE_SPACE))
			Playing = true;
	}*/
	
	/*ImGui::Text("=================");
	ImGui::Text("|Spline Controls|");
	ImGui::Text("================="); 
	ImGui::Text("o Create up to 8 control points by clicking");
	ImGui::Text("  with the mouse middle button");
	ImGui::Text("Reset Spline -> "); ImGui::SameLine();
	if (ImGui::Button("Reset"))
		SplineReset = true;
	ImGui::Text("Hide Spline -> "); ImGui::SameLine();
	if (SplineHidden)
	{
		if (ImGui::Button("Show"))
			SplineHidden = false;
	}
	else
	{
		if (ImGui::Button("Hide"))
			SplineHidden = true;
	}

	ImGui::Text(" ");
	ImGui::Text(" ");*/
	/*ImGui::SliderFloat("Character Peace on path-> ", (float*)&PathPeace, 1.0f, 5.0f);
	ImGui::Text("");*/

	ImGui::Text("==================");
	ImGui::Text("|Objects Controls|");
	ImGui::Text("==================");

	if (ObjectsPosition.size() > 0)
	{
		
		for (size_t i = 0; i < ObjectsPosition.size(); i++)
		{
			std::string name = " ----- GameObject: " + ObjectsName[i] + " ----- ";
			std::string objName = ObjectsName[i];
			GameObject* go = resourceManager->GetGameObjectByName(objName);
			std::string chkbxlbl = "Select " + std::to_string(i);
			ImGui::Checkbox(chkbxlbl.c_str(), &go->GetGLObject()->IsSelected); //ImGui::SameLine(300);
			if (go->GetGLObject()->IsSelected)
			{
				int a = 0;
				a++;
			}

			std::string label;
			ImGui::Text(name.c_str());
			ImGui::Text("-------------------------------------------------------");
			ImGui::Text("x                  Y                  Z");

			if (ObjectsName[i] != "Target")
			{
				label = "Position_" + ObjectsName[i] + "_" + std::to_string(i);
				ImGui::SliderFloat3(label.c_str(), (float*)&(*ObjectsPosition[i]), -1000.0f, 1000.0f);
			}		
			label = "Rotation_" + ObjectsName[i] + "_" + std::to_string(i);
			ImGui::SliderFloat3(label.c_str(), (float*)&(*ObjectsRotation[i]), -360.0f, 360.0f);
			/*label = "Scale_" + ObjectsName[i] + "_" + std::to_string(i);
			ImGui::SliderFloat(label.c_str(), (float*)&(*ObjectsScale[i]).x, 0.0f, 2.0f);*/
			ImGui::Text("");
			//
		}
	}

	
	//
}

// 2. Show another simple window, this time using an explicit Begin/End pair
void UIManager::UISimpleWindow()
{
	if (show_another_window)
	{
		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Another Window", &show_another_window);
		ImGui::Text("Hello");
		ImGui::End();
	}
}

// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
void UIManager::UITestWindow()
{
	if (show_test_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void UIManager::UISceneClearColor()
{
	//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void UIManager::UIRender()
{
	ImGui::Render();
}


void UIManager::TestImGui()
{

	//std::cout << active << std::endl;
	if (active)
	{
		UIShowSampleWindow();
		UISimpleWindow();
		UITestWindow();
	}
	
}

void UIManager::UIActivateSwitch()
{
	if (inputManager->IsKeyPressed(SDL_SCANCODE_1))
		active = true;
	if (inputManager->IsKeyPressed(SDL_SCANCODE_2))
		active = false;
}


void UIManager::PushDebugMessage(std::string msg)
{
	debugMessages.push_back(msg + "\n");
}

int UIManager::GetShadingMode()
{
	return shading;
}

int UIManager::IsDeferredDebugMode()
{
	return deferred_debug;
}