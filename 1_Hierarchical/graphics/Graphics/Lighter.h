#pragma once
#include "LightPointComponent.h"
class Lighter
{
	

public:
	Lighter();
	~Lighter();

	void ProcessLights(int shaderID, LightPointComponent* light);
};

