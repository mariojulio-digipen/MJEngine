#include "GameObject.h"
#include "GLObject.h"
#include "Component.h"

#include <iostream>

GameObject::GameObject() : id(0)
{
	this->IsBone = false;
	glObject = new GLObject;
	glObject->ActiveColor = true;
	glObject->DebugMode = false;
}


GameObject::~GameObject()
{
	delete glObject;
}

// for debugging in console purposes only
void GameObject::showDebugMessage(const char* message)
{
	int i;
	std::cout << message << " in game object: " << this->name.c_str();
	std::cout << "\nPress any key to exit the app\n";
	std::cin >> i;
	exit(EXIT_FAILURE);
}

std::string GameObject::readGameObjectFile(const char * fileName)
{
	std::string line;
	std::stringstream fileString;
	std::ifstream file(fileName);
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			fileString << line << "\n";
		}
		file.close();
	}
	else
	{
		printf("Unable to open model file.");
	}
	return fileString.str();
}

Shader* GameObject::GetDefaultShader() const
{
	return glObject->Shader;
}

void GameObject::AddComponent(Component* component)
{
	components.push_back(component);
	if (components.size() > 10)
		showDebugMessage("Cannot add more than 10 components");
	component->owner = this;
}

std::vector<Component*>* GameObject::GetPointerToAllComponents()
{
	return &components;
}

void GameObject::InitializeAllComponents()
{
	std::vector<Component*>::iterator it = components.begin();
	while(it != components.end())
	{
		if (!*it)
			showDebugMessage("Error: Trying to initialize a NULL component");
	
		if (!(*it)->Init())
			showDebugMessage("Something went wrong while initializing one component");
		it = it + 1;
	}
}

void GameObject::UpdateAllComponents()
{
	if (!IsAlive)
		return;

	std::vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if (!*it)
			showDebugMessage("Error: Trying to update a NULL component");
		(*it)->Update();
		it = it + 1;
	}
}

void GameObject::SerializeAllComponents(std::string* word, std::istringstream* objcontentStream)
{
	std::vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if (!*it)
			showDebugMessage("Error: Trying to serializse a NULL component");
		(*it)->Serialize(objcontentStream);
		it = it + 1;
	}
}

void GameObject::HandleEvent(const Event* const evt)
{
	std::vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if (!*it)
			showDebugMessage("Error: Trying to handle events with a NULL component");
		(*it)->HandleEvent(evt);
		it = it + 1;
	}
}

void GameObject::Start()
{
	std::vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if (!*it)
			showDebugMessage("Error: Trying to handle events with a NULL component");
		(*it)->Start();
		it = it + 1;
	}
}

void GameObject::CompleteExtraSetup()
{
	std::vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if (!*it)
			showDebugMessage("Error: Trying to serializse a NULL component");
		(*it)->CompleteExtraSetup();
		it = it + 1;
	}
}

void GameObject::DestroyAllComponents()
{
	std::vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if (!*it)
			showDebugMessage("Error: Trying to delete component that does not exist");
		(*it)->Destroy();
		delete (*it);
		it = it + 1;
	}
	components.clear();
}


void GameObject::DestroyComponent(Component* componentToDestroy)
{
	if (!componentToDestroy)
		showDebugMessage("Error: Trying to delete component that does not exist");
	componentToDestroy->Destroy();
	delete componentToDestroy;
}

GLObject * GameObject::GetGLObject() const
{
	return glObject;
}