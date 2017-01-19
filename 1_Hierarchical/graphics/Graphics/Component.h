#pragma once

#include <map>
#include <iostream>
#include <fstream>

class GameObject;
class Event;


class Component
{
private:
	//typedef std::unordered_map<std::string, int> component_types;
	friend class GameObject;
	GameObject* owner;
	
protected:
	std::string readGameObjectFile(const char* fileName);

public:
	Component();
	virtual ~Component();

	virtual bool Init() { return true; }
	virtual void Update() {}
	virtual void Destroy() {}
	virtual void Serialize(std::istringstream* objcontentStream) {}
	virtual void CompleteExtraSetup() {}
	virtual void HandleEvent(const Event* const evt) {}

	GameObject* GetOwner() const { return owner; }
};

