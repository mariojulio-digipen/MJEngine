#pragma once
#include <string>
#include <vector>
#include <map>

class GLObject;
class Component;
class Camera2D;
class Camera3D;
class Shader;
class ResourceManager;
class Event;
class GameObject
{
private:

	static const int NUM_MAX_COMPONENTS = 10;
	std::vector<Component*> components;
	int id;
	std::string name;
	GLObject* glObject;

	void showDebugMessage(const char* message);
	std::string readGameObjectFile(const char* fileName);

	// hierarchy system
	std::map<std::string, GameObject*> children;
	GameObject* parent = NULL;

public:

	// hierarchy system
	void SetChildren(GameObject* gameobject)
	{ 
		if(gameobject)
			children.insert(std::make_pair(gameobject->GetName(), gameobject));
	}

	GameObject* GetChild(std::string name) const
	{
		std::map<std::string, GameObject*>::const_iterator it = children.find(name);
		if (it != children.end())
			return it->second;
		return NULL;
	}

	

	void SetParent(GameObject* gameobject) { parent = gameobject; }
	GameObject* GetParent() const { return parent; }


	bool IsAlive = true;
	bool IsDeferred = false;
	bool NeedsDeletion = false;

	Camera2D* MainCamera2D = NULL;
	Camera3D* MainCamera3D = NULL;

	

	GameObject();
	~GameObject();

	void SetId(int id) { this->id = id; }
	void SetName(std::string name) { this->name = name; }

	int GetId() const { return id; }
	std::string GetName() const { return name; }
	void SetNewName(std::string name) { this->name = name; }
	Shader* GetDefaultShader() const;
	GLObject* GetGLObject() const;
	void AddComponent(Component* component);
	std::vector<Component*>* GetPointerToAllComponents();
	void InitializeAllComponents();
	void UpdateAllComponents();
	void SerializeAllComponents(std::string* word, std::istringstream* objcontentStream);
	void HandleEvent(const Event* const evt);
	void Start();
	void CompleteExtraSetup();
	void DestroyAllComponents();
	void DestroyComponent(Component* componentToDestroy);


	template<class T>
	T* FindComponent()
	{
		std::vector<Component*>::iterator it = components.begin();
		while (it != components.end())
		{
			T* ret = dynamic_cast<T*>((*it));
			if (ret != 0)
				return ret;
			it = it + 1;
		}
		return NULL;
	}


	// members for bone rendering
	bool IsBone;
	bool IsMesh;
};

