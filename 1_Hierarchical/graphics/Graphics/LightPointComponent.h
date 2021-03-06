// LightPoint.H
// Header autogenerated by MJEngine automation tools
// WARNING: Do not modify autogenerated comments, automation 
// tools depend on them.

// Edition author: mzaldivar

#pragma once
#include "Component.h"

#include <glm.hpp>
class LightPointComponent : public Component
{
private:
	// Start Private properties list
	glm::vec3 lighPosition;
	glm::vec3 lightAmbient;
	glm::vec3 lightIntensity;
	glm::vec3 lightColor;

	// End Private properties list
	
	// Start private user defined functions

		// TODO - declare user defined private functions

	// End private user defined functions

public:

	// Start Public properties list
	glm::vec3 GetLightColor() { return lightColor; }
	glm::vec3 GetLightPosition() { return lighPosition; }
	glm::vec3 GetLightIntensity() { return lightIntensity; }
	glm::vec3 GetLightAmbient() { return lightAmbient; }
	
	void SetLightColor(glm::vec3 color) { lightColor = color; }
	void SetLightColor(float r, float g, float b) { lightColor = glm::vec3(r, g, b); }

		// TODO - declare public members

	// End Private properties list

	LightPointComponent();
	~LightPointComponent();

	// Start implementation functions
	bool Init();
	void Update();
	void Serialize(std::istringstream* objcontentStream);
	// End implementation functions

	// Start public user defined functions

		// TODO - declare user defined public functions

	// End public user defined functions
};
