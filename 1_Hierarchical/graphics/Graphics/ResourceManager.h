#pragma once
#include <string>
#include <vector>
#include <map>
#include <list>
#include <SDL_atomic.h>

#include "GameObject.h"

//#include "MJFBXImporter.h"

//class GameObject;
class GLObject;
class Camera2D;
class Camera3D;
class Bone;
class Mesh;
class MJFBXImporter;
class ResourceManager
{

private:

	short unsigned int objectsCounter;

	std::string masterRootBoneName;

	std::vector<GameObject*> gameObjects2D;
	std::vector<GameObject*> gameObjects3D;
	std::vector<GameObject*> gameObjects2D_Spawned;
	std::vector<GameObject*> gameObjects3D_Spawned;


	std::vector<GameObject*> gameObjects3D_Deferred;

	std::vector<int> ids;
	std::vector<int> texIds;

	std::map<std::string, GameObject*> gameObjects2D_map;
	std::map<std::string, GameObject*> gameObjects3D_map;

	std::string gameObjecFileContent;
	MJFBXImporter* MJEngineFBXImporter;

	std::string readFile(const char* fileName);
	void readSingleVertexData(GLObject* glo);
	void readDebugLinesData(GLObject* glo);
	void readModelData(GLObject* glo, std::string modelName, std::string objFile, GameObject* go = NULL);
	void storeDataInGPU(GameObject* go);
	void storeDebugDataInGPU(GLObject* glo);
	void attachShaders2D(GLObject* glo);
	void attachShaders3D(GLObject* glo);
	void convertGLTransform2DToComponent(GLObject* glo, GameObject* go);
	void convertGLTransform3DToComponent(GLObject* glo, GameObject* go);
	void attachTexture(std::vector<GameObject*>::iterator goit); 
	void setGameObjectName(std::string* name, unsigned short int* id, std::string* filepath);

	
	void sendGameObjectsToGPU();

	void setTexIds(std::vector<GameObject*>::iterator goit, GLObject* glo);
	void cleanObjectModelMemory(GLObject* glo);
	void createInitialFromOBJ(std::string name);
	void createInitialFromFBX(std::string name);

	void createBoneAsGameObject(Bone* bone);
	void createAnimationMeshAsGameObject(Mesh* mesh);

public:
	ResourceManager();
	~ResourceManager();


	void GenerateGameObjects();
	void Start();
	void DeleteGameObjects();
	GameObject* CreateOneGameObject(std::string name, bool spawned = false, bool deferred = false);
	void Update3DGameObjects();
	void Update3DDeferredObjects();
	void Update2DGameObjects();
	//void UpdatePseudoGameObject(); // extra objects that do not follow general transformations

	void AttachGameObjectToSystem(GameObject* go);

	std::vector<GameObject*> Get2DGameObjects() { return gameObjects2D; }
	std::vector<GameObject*> Get3DGameObjects() { return gameObjects3D; }

	std::map<std::string, GameObject*> Get3DGameObjectsMap() { return gameObjects3D_map; }

	void createGameObjectsInCPU();

	void createImmediately(GameObject* go);
	void createNextFrame(GameObject* go);
	
	GameObject* SpawnNewGameObject(std::string name);
	GameObject* SpawnNewGameObjectFromArchetype(std::string name);
	GameObject* SpawnNewGameObjectFromArchetypeForDeferredRendering(std::string name);
	GameObject* GetGameObjectByName(std::string name);
	
	template<typename T>
	T* AttachComponent(GameObject* go)
	{
		T* component = new T;
		go->AddComponent(component);
		return component;
	}



};

