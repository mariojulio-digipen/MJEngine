// SpawnerComponent.H
// Header autogenerated by MJEngine automation tools
// WARNING: Do not modify autogenerated comments, automation 
// tools depend on them.

// Edition author: mzaldivar

#pragma once
#include "Component.h"
#include "InputManager.h"
#include "ResourceManager.h"

extern InputManager* inputManager;
extern ResourceManager* resourceManager;

class SpawnerComponent : public Component
{
private:
	// Start Private properties list

		// TODO - declare private members
	bool firstLightSpawned;
	// End Private properties list
	
	// Start private user defined functions

		// TODO - declare user defined private functions

	// End private user defined functions

public:

	// Start Public properties list

		// TODO - declare public members

	// End Private properties list

	SpawnerComponent();
	~SpawnerComponent();

	// Start implementation functions
	bool Init();
	void Update();
	void Serialize(std::istringstream* objcontentStream);
	// End implementation functions

	// Start public user defined functions

		// TODO - declare user defined public functions

	// End public user defined functions
};
