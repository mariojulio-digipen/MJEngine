#include "ResourceManager.h"
#include "GameObject.h"
#include "GLObject.h"
#include "PainterComponent.h"
#include "Shader.h"
#include "ModelLoader.h"
#include "RenderManager.h"
#include "Render2D.h"
#include "Render3D.h"
#include "LineComponent.h"
#include "KeyFrameAnimationComponent.h"
#include "MoveAlongPathComponent.h"

#include <unordered_map>
#include <algorithm>
#include <SDL.h>
#include <SDL_thread.h>

#include "Component.h"
#include "TransformComponent.h"
#include "ComponentAttacher.h"
#include "FrameRateController.h"
#include "UIUpdaterComponent.h"

#include "VQS.h"


#include "AnimationData.h"

#include "UIManager.h"

#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <gtx\euler_angles.hpp>
#include <glm.hpp>


extern Shader* shader;
extern RenderManager* renderManager;
extern FrameRateController* frameRateController;
extern UIManager* uiManager;
extern SDL_Window* mainWindow;

int parallelCreateGameObjectsInCPU(void* ptr);

ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
}

std::string ResourceManager::readFile(const char* fileName)
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
		printf("Unable to open game object file.");
	}
	return fileString.str();
}

SDL_atomic_t stuffIsReady;
int parallelCreateGameObjectsInCPU(void* ptr)
{
	ResourceManager* mngr = static_cast<ResourceManager*>(ptr);
	mngr->createGameObjectsInCPU();
	GLsync fenceid = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	GLenum result = false;
	while (true)
	{
		glClientWaitSync(fenceid, GL_SYNC_FLUSH_COMMANDS_BIT, GLuint(5000000000));
		if (result != GL_TIMEOUT_EXPIRED)
			break;
	}
	SDL_AtomicIncRef(&stuffIsReady);
	return 0;
}

void ResourceManager::GenerateGameObjects()
{
	GameObject* loading = CreateOneGameObject("UILoading");
	readModelData(loading->GetGLObject(), loading->GetName(), loading->GetGLObject()->Model->modelPath);
	storeDataInGPU(loading);
	
	std::vector<GameObject*> dummy;dummy.push_back(loading);
	std::vector<GameObject*>::iterator dummyIt = dummy.begin();
	attachTexture(dummyIt);
	
	renderManager->GetRender2D()->Use();
	frameRateController->SetTargetFrameRate(60.0f);
	SDL_Thread* thread = SDL_CreateThread(parallelCreateGameObjectsInCPU, "Loading_Thread", this);
	while (true)
	{
		frameRateController->FrameStart();
		renderManager->Clear();
		shader->UseProgram();
		loading->GetDefaultShader()->Enable2D();
		loading->UpdateAllComponents();
		loading->GetDefaultShader()->Disable2D();
		SDL_GL_SwapWindow(mainWindow);
		frameRateController->FrameEnd();
		if (SDL_AtomicGet(&stuffIsReady) == 1)
			break;
	}
	int threadReturnValue;
	SDL_WaitThread(thread, &threadReturnValue);
	loading->DestroyAllComponents();
	delete loading;

	sendGameObjectsToGPU();
}





void ResourceManager::createGameObjectsInCPU()
{
	//read scene file:
	std::vector<std::string> sceneGameObjects;
	std::string fileContent = readFile("InitialObjects.txt");
	std::istringstream contentStream(fileContent.c_str());
	std::string word;
	std::string line;
	while (contentStream >> word)
	{
		if (word == "#")
		{
			std::getline(contentStream, line);
			continue;
		}
		if (word == "o")
		{
			std::getline(contentStream, line);
			line.erase(
				std::remove_if(line.begin(),
					line.end(), isspace),
				line.end()
			);
			sceneGameObjects.push_back(line);
			continue;
		}
		if (word == "f")
		{
			std::getline(contentStream, line);
			line.erase(
				std::remove_if(line.begin(),
					line.end(), isspace),
				line.end()
			);
			sceneGameObjects.push_back(line + ".fbx");
			continue;
		}
	}


	// iterate objects in scene files. This sections decides whether to create
	// a game object using an OBJ (1-1) source or a different method using FBX(1-N) source
	std::vector<std::string>::iterator it = sceneGameObjects.begin();
	for (; it != sceneGameObjects.end(); it++)
	{
		if ((*it).find(".fbx") == std::string::npos) // it is not an fbx source, then only one obj is stored.
			createInitialFromOBJ((*it));
		else
			createInitialFromFBX((*it)); // it is an fbx source, 1 or many game objects will be stored.
	}

	std::vector<GameObject*>::iterator GO2D = gameObjects2D.begin();
	while (GO2D != gameObjects2D.end())
	{
		std::string modelpath = (*GO2D)->GetGLObject()->Model->modelPath;
		if (modelpath.find("debug_") != -1)
			readDebugLinesData((*GO2D)->GetGLObject());
		else
			readModelData((*GO2D)->GetGLObject(), (*GO2D)->GetName(), (*GO2D)->GetGLObject()->Model->modelPath);
		GO2D = GO2D + 1;
	}

	std::vector<GameObject*>::iterator GO3D = gameObjects3D.begin();
	while (GO3D != gameObjects3D.end())
	{
		GLObject* glb = (*GO3D)->GetGLObject();
		std::string name = (*GO3D)->GetName();

		std::string path;
		ModelLoader* modeldata = glb->Model;
		if (modeldata) // Only obj meshes have modeldata. Fbx meshes uses other type of mesh models
			path = modeldata->modelPath;

		// for animmesh debug
		if (name == "Boy01_Body_Geo")
		{
			int a = 0;
			a++;
		}

		// for animmesh debug
		if (name == "Hips")
		{
			int a = 0;
			a++;
		}

		if (path.find("scene") != -1)
			readSingleVertexData((*GO3D)->GetGLObject());
		else
			readModelData(glb, name, path, (*GO3D));
		GO3D = GO3D + 1;
	}

}

void ResourceManager::createInitialFromOBJ(std::string name)
{
	GameObject* go = CreateOneGameObject(name);
	go->CompleteExtraSetup();

	ids.push_back(ids.size() + 1);
	go->SetId(ids.back());
}

void ResourceManager::createAnimationMeshAsGameObject(Mesh* mesh)
{
	GameObject* meshGO = new GameObject;
	GLObject* glo = meshGO->GetGLObject();
	meshGO->IsAlive = true;
	meshGO->IsBone = false;
	meshGO->IsMesh = true;
	glo->DrawBone = false;
	glo->DrawAnimMesh = true;
	meshGO->SetName(mesh->GetName());
	glo->Name = meshGO->GetName();
	glo->Bonetype = BONE_TYPE::BONE_MAX;
	glo->MeshClusterMode = mesh->GetClusterMode();
	glo->MeshSkinningType = mesh->GetSkinningType();
	
	// Don't think I need keyframes here...
	// TODO later if required
	//size_t num_keyframes = mesh->KeyFrames.size();

	meshGO->MainCamera2D = NULL;
	meshGO->MainCamera3D = renderManager->GetRender3D()->CurrentCamera;
	attachShaders3D(glo);

	convertGLTransform3DToComponent(glo, meshGO);

	glo->Shader->SetTranslucent(false);
	glo->Model = NULL;
	glo->FModel = MJEngineFBXImporter;

	// setup object id
	ids.push_back(ids.size() + 1);
	meshGO->SetId(ids.back());

	assembleComponent<PainterComponent>(meshGO);
	// TODO: add as many components are necessary to make skinning work!!!
	//..
	//...
	assembleComponent<UIUpdaterComponent>(meshGO);
	meshGO->InitializeAllComponents();
	meshGO->CompleteExtraSetup();

	// set up initial transform data
	TransformComponent* transform = meshGO->FindComponent<TransformComponent>();
	transform->SetPosition(mesh->GetInitialTransform().position);
	transform->SetRotation(mesh->GetInitialTransform().rotation);
	transform->SetScale(mesh->GetInitialTransform().scale);

	glo->Transform->VQS_ModelToWorldForAnimMesh = mesh->GetInitialTransform().VQS_ModelToWorld;
	glo->Transform->VQS_ScalerForAnimMesh = 1.0f;//mesh->GetInitialTransform().VQS_ModelToWorld.ScaleUniform;//mesh->KeyFrames[0].VQS_Transform.ScaleUniform;


	// we will assume that the meshes are sons of the root bone, 
	// to apply transformations accordingly
	std::map<std::string, GameObject*>::iterator gos3D = gameObjects3D_map.find(masterRootBoneName);
	if (gos3D != gameObjects3D_map.end())
	{
		meshGO->SetParent(gos3D->second);
	}

	createImmediately(meshGO);

	std::vector<MeshClusterData>* clusterData = mesh->GetClusterBoneData();
	for (auto data : (*clusterData))
	{
		std::string infBoneName = data.influencerBoneName;
		glm::mat4 matrix = data.globalBindPoseMatrix;
		meshGO->GetGLObject()->Transform->DeformMatrices.insert(std::make_pair(infBoneName, matrix));
	}
	//meshGO->GetGLObject()->Transform->DeformMatrices = clusterData->

}

void ResourceManager::createBoneAsGameObject(Bone* bone)
{
	GameObject* boneGO = new GameObject;
	GLObject* glo = boneGO->GetGLObject();
	boneGO->IsAlive = true;
	boneGO->IsBone = true;
	glo->DrawBone = true;
	boneGO->SetName(bone->Name);
	glo->Name = bone->Name;
	glo->Bonetype = bone->Type;
	if (glo->Bonetype == BONE_TYPE::ROOT)
		masterRootBoneName = glo->Name;

	size_t num_keyframes = bone->KeyFrames.size();
	glo->KeyFramesData.resize(num_keyframes);
	for (size_t i = 0; i < num_keyframes; i++)
	{
		glo->KeyFramesData[i].index = bone->KeyFrames[i].index;
		//glo->KeyFramesData[i].Matrix = bone->KeyFrames[i].Matrix;
		glo->KeyFramesData[i].time = bone->KeyFrames[i].time;
		glo->KeyFramesData[i].vqs = bone->KeyFrames[i].VQS_Transform;
	}

	boneGO->MainCamera2D = NULL;
	boneGO->MainCamera3D = renderManager->GetRender3D()->CurrentCamera;
	attachShaders3D(glo);
	convertGLTransform3DToComponent(glo, boneGO);
	glo->Shader->SetTranslucent(false);
	glo->Model = NULL;
	glo->FModel = MJEngineFBXImporter;
	
	// setup object id
	ids.push_back(ids.size() + 1);
	boneGO->SetId(ids.back());


	
	
	assembleComponent<PainterComponent>(boneGO);

	
	assembleComponent<KeyFrameAnimationComponent>(boneGO);

	assembleComponent<LineComponent>(boneGO);
	// use the root node instead of a specific name
	//if (glo->Name == "LeftHand"/*glo->Name == "LeftShoulder"*//* || glo->Name == "Hips"*/)
		//assembleComponent<UIUpdaterComponent>(boneGO);
	//assembleComponent<UIUpdaterComponent>(boneGO);
	/*if (glo->Name == "Hips")
		assembleComponent<UIUpdaterComponent>(boneGO);*/

	if(glo->Bonetype == BONE_TYPE::ROOT)
	{
		//assembleComponent<UIUpdaterComponent>(boneGO);
		assembleComponent<MoveAlongPathComponent>(boneGO);
		//assembleComponent<Move3DComponent>(boneGO);
	}
	boneGO->InitializeAllComponents();
	boneGO->CompleteExtraSetup();

	// set up initial transform data
	TransformComponent* transform = boneGO->FindComponent<TransformComponent>();
	transform->SetPosition(bone->Transform.position);
	transform->SetRotation(bone->Transform.rotation);
	transform->SetScale(bone->Transform.scale);
	//transform->SetScale(glm::vec3(0.144f, 0.144f, 0.144f));
	//glo->Transform->ModelToWorld = bone->Transform.ModelToWorld;
	//glo->Transform->ModelToWorldForBone = bone->Transform.ModelToWorld;
	glo->Transform->VQS_ModelToWorldForBone = bone->Transform.VQS_ModelToWorld;
	glo->Transform->VQS_ScalerForBone = bone->KeyFrames[0].VQS_Transform.ScaleUniform;
	//glo->Transform->VQS_ModelToWorldForBone = glo->KeyFramesData[i].vqs;
	//glo->Transform->WorldToModel = bone->Transform.WorldToModel;
	//glo->Transform->WorldToModel = bone->Transform.ModelToWorld;


	createImmediately(boneGO);
}


void ResourceManager::createInitialFromFBX(std::string name)
{

	std::string relativePath(SDL_GetBasePath());
	std::string modelPath(relativePath + "ArtAssets\\exported\\FBX\\" + name);
	
	MJEngineFBXImporter = new MJFBXImporter;
	MJEngineFBXImporter->InitializeSDKManager();
	MJEngineFBXImporter->ImportItem(MODEL);
	MJEngineFBXImporter->ImportItem(ANIMATION);
	MJEngineFBXImporter->Create(modelPath);
	MJEngineFBXImporter->CreateScene();

	AnimationData* animData = MJEngineFBXImporter->GetAnimationObject();
	if (animData->GetAnimationStackCount() <= 0)
	{
		std::cout << "- NO ANIMATION DATA TO LOAD - " << std::endl;
		return;
	}
		
	std::cout << "LOADING ANIMATION DATA " << std::endl;
	std::cout << "=======================" << std::endl;

	std::cout << "Animation stack #: " << animData->GetAnimationStackCount() << std::endl;

	


	// read what is so called bones
	bool ok = animData->ReadBones(); // I assume if has bind pose, it's an animation...
	if (ok)
	{
		int numberofRoots = 0;;
		for (auto pbone : animData->BonesListInDepthFirst)
		{
			Bone* bone = &pbone;
			createBoneAsGameObject(bone);
			if (bone->Type == BONE_TYPE::ROOT)
				numberofRoots++;
		}
		numberofRoots = 0;
	}
	else // ... otherwise, it's a simple mesh import in fbx format
	{
		//return;


		// I don't want mesh data yet

		// TODO: need to update ReadNodes so I only read mesh-type nodes, 
		// and avoid skeleton-type
		MJEngineFBXImporter->ReadNodes();
		size_t goNum = MJEngineFBXImporter->NodesMeshData.size();
		std::unordered_map<std::string, std::vector<VertexData>>::iterator goDataIt;
		goDataIt = MJEngineFBXImporter->NodesMeshData.begin();

		while (goDataIt != MJEngineFBXImporter->NodesMeshData.end())
		{
			GameObject* fbxgo = new GameObject;
			fbxgo->IsAlive = true;
			std::string goName = goDataIt->first;
			fbxgo->SetName(goName);
			fbxgo->GetGLObject()->Name = goName;
			fbxgo->MainCamera2D = NULL;
			fbxgo->MainCamera3D = renderManager->GetRender3D()->CurrentCamera;
			attachShaders3D(fbxgo->GetGLObject());
			convertGLTransform3DToComponent(fbxgo->GetGLObject(), fbxgo);
			fbxgo->GetGLObject()->Shader->SetTranslucent(false);
			// Model is only for obj loading
			fbxgo->GetGLObject()->Model = NULL;
			fbxgo->GetGLObject()->FModel = MJEngineFBXImporter;

			// However, we still need to read the components configuration
			// read object file
			std::string::iterator nameIt = name.begin();
			std::string noExt = "";
			while (nameIt != name.end())
			{
				if ((*nameIt) == '.')
					break;
				noExt += (*nameIt);
				nameIt++;
			}
			std::string filepath = noExt + "-" + goName + ".txt";
			std::string objectFileContent = readFile(filepath.c_str());

			// if empty, it means all links will have default 
			// components: Transform and Painter components
			if (objectFileContent == "")
			{
				assembleComponent<PainterComponent>(fbxgo);
				//assembleComponent<UIUpdaterComponent>(fbxgo);
				fbxgo->InitializeAllComponents();
			}
			else
			{
				std::istringstream objcontentStream(objectFileContent.c_str());
				std::string word;
				std::string line;
				while (objcontentStream >> word)
				{
					if (word == "#" || word == "objectname" ||
						word == "rendermode" || word == "modelpath" ||
						word == "debug_aabb" || word == "debug_circle")
					{
						std::getline(objcontentStream, line);
						continue;
					}

					if (word == "c")
					{
						std::getline(objcontentStream, line);
						line.erase(
							std::remove_if(line.begin(),
								line.end(), isspace),
							line.end()
						);
						attachComponents(line, fbxgo);
						continue;
					}

					if (word == "compprops")
					{
						std::getline(objcontentStream, line);
						std::getline(objcontentStream, line);
						fbxgo->InitializeAllComponents();
						fbxgo->SerializeAllComponents(&word, &objcontentStream);
						continue;
					}

				}
			}



			fbxgo->CompleteExtraSetup();

			ids.push_back(ids.size() + 1);
			fbxgo->SetId(ids.back());

			createImmediately(fbxgo);

			goDataIt++;
		}
		int i = 2;
		gameObjects3D;
	}

	
	// read the meshes, if there is any
	size_t animMeshes = animData->AnimationMeshes.size();
	if (animMeshes > 0)
	{
		for (auto pmesh : animData->AnimationMeshes)
		{
			Mesh* mesh = &pmesh;
			createAnimationMeshAsGameObject(mesh);


			// One I have everything setup in the game object, 
			// bone influence id and matrix are set.
			/*std::vector<GameObject*>::iterator goIT = gameObjects3D.begin();
			while (goIT != gameObjects3D.end())
			{
				GLObject* glo = (*goIT)->GetGLObject();
				MeshClusterData* clusterData = mesh->GetClusterBoneData(glo->Name);
				if (clusterData != NULL)
				{
					glo->Transform->DeformID = clusterData->influencerBoneId;
					glo->Transform->DeformMatrix = clusterData->globalBindPoseMatrix;
				}
				else
				{
					int a = 0; a++;
				}

				goIT++;
				
			}
*/
			int a = 0; a++;
			
		}

	}

	

	
	
	return;
}

void ResourceManager::sendGameObjectsToGPU()
{
	std::vector<GameObject*>::iterator GO2D = gameObjects2D.begin();
	std::vector<GameObject*>::iterator GO3D = gameObjects3D.begin();
	while (GO2D != gameObjects2D.end())
	{
		std::string name = (*GO2D)->GetName();
		if (name.find("Debug") != -1)
		{
			storeDebugDataInGPU((*GO2D)->GetGLObject());
		}
		else
		{
			storeDataInGPU((*GO2D));
			attachTexture(GO2D);
		}
		
		GO2D = GO2D + 1;
	}
	while (GO3D != gameObjects3D.end())
	{
		storeDataInGPU((*GO3D));
		if(!(*GO3D)->GetGLObject()->IsScenePoint)
			attachTexture(GO3D);
		GO3D = GO3D + 1;
	}

	// clean fbx import memory model if exists
	if (MJEngineFBXImporter)
	{

		MJEngineFBXImporter->DestroySDKObjects();
		delete MJEngineFBXImporter;
	}
	
}

void ResourceManager::AttachGameObjectToSystem(GameObject* go)
{
	go->CompleteExtraSetup();
	ids.push_back(ids.size() + 1);
	go->SetId(ids.back());


	// generate in CPU
	readModelData(go->GetGLObject(), go->GetName(), go->GetGLObject()->Model->modelPath);

	// send to GPU
	std::vector<GameObject*> dummy; dummy.push_back(go);
	std::vector<GameObject*>::iterator dummyIt = dummy.begin();

	storeDataInGPU(go);
	attachTexture(dummyIt);
}

void ResourceManager::setGameObjectName(std::string* name, unsigned short int* id, std::string* filepath)
{
	if ((*name) == "TextCanvas")
		(*filepath) = (*name) + std::to_string(*id) + ".txt";
	else
		(*filepath) = (*name) + ".txt";
}

void ResourceManager::DeleteGameObjects()
{
	std::vector<GameObject*>::iterator GO3D = gameObjects3D.begin();
	while (GO3D != gameObjects3D.end())
	{
		if ((*GO3D) != NULL)
		{
			(*GO3D)->DestroyAllComponents();
			delete (*GO3D);
		}
		GO3D = GO3D + 1;
	}

	std::vector<GameObject*>::iterator GO2D = gameObjects2D.begin();
	while (GO2D != gameObjects2D.end())
	{
		if ((*GO2D) != NULL)
		{
			(*GO2D)->DestroyAllComponents();
			delete (*GO2D);
		}
		GO2D = GO2D + 1;
	}
}

GameObject* ResourceManager::CreateOneGameObject(std::string name, bool spawned, bool deferred)
{
	GameObject* go = new GameObject;
	go->IsAlive = false;

	// read object file
	std::string filepath = name + ".txt";
	std::string objectFileContent = readFile(filepath.c_str());

	
	std::istringstream objcontentStream(objectFileContent.c_str());
	std::string word;
	std::string line;

	if (name == "TextCanvas")
		name = name + std::to_string(objectsCounter);
	go->SetName(name);
	go->GetGLObject()->Name = name;
	go->IsDeferred = deferred;

	while (objcontentStream >> word)
	{
		if (word == "#")
		{
			std::getline(objcontentStream, line);
			continue;
		}

		if (word == "objectname")
		{
			std::getline(objcontentStream, line);
			line.erase(
				std::remove_if(line.begin(),
					line.end(), isspace),
				line.end()
			);
			continue;
		}

		// this is to create a skeleton made of game objects
		if (word == "skeleton")
		{
			std::getline(objcontentStream, line);
			line.erase(
				std::remove_if(line.begin(),
					line.end(), isspace),
				line.end()
			);

			if (line == "root")
			{
				go->SetParent(NULL);
				go->GetGLObject()->Bonetype = BONE_TYPE::ROOT;
			}
				

			if (line == "limb")
			{
				go->GetGLObject()->Bonetype = BONE_TYPE::LIMB;

				objcontentStream >> word;
				if (word == "parent")
				{
					std::getline(objcontentStream, line);
					line.erase(
						std::remove_if(line.begin(),
							line.end(), isspace),
						line.end()
					);
					GameObject* parent = GetGameObjectByName(line);
					go->SetParent(parent);
				}
					
			}
			
			//glo->Transform->VQS_ModelToWorldForBone

			go->IsBone = true;
			go->GetGLObject()->DrawAnimMesh = false;
			go->GetGLObject()->BoneHandMade = true;
			go->GetGLObject()->DrawBone = true;

			continue;
		}

		if (word == "rendermode")
		{
			std::getline(objcontentStream, line);
			std::string lineContent = line;
			std::istringstream lineStream(lineContent.c_str());
			std::string word;
			std::string prop;
			while (lineStream >> word)
			{
				std::getline(lineStream, prop);
				if (word.find("OFF_3D") != -1)
				{
					go->MainCamera2D = NULL;
					go->MainCamera3D = renderManager->GetRender3D()->CurrentCamera;
					attachShaders3D(go->GetGLObject());
					convertGLTransform3DToComponent(go->GetGLObject(), go);
					go->GetGLObject()->Shader->SetTranslucent(false);
					go->IsAlive = false;
					//go->IsVisible = false;
					continue;
				}
				if (word.find("ON_3D") != -1)
				{
					go->MainCamera2D = NULL;
					go->MainCamera3D = renderManager->GetRender3D()->CurrentCamera;
					attachShaders3D(go->GetGLObject());
					convertGLTransform3DToComponent(go->GetGLObject(), go);
					go->GetGLObject()->Shader->SetTranslucent(false);
					go->IsAlive = true;
					continue;
				}
				if (word.find("OFF_2D") != -1)
				{
					go->MainCamera2D = renderManager->GetRender2D()->CurrentCamera;
					go->MainCamera3D = NULL;
					attachShaders2D(go->GetGLObject());
					convertGLTransform2DToComponent(go->GetGLObject(), go);
					go->IsAlive = false;
					continue;
				}
				if (word.find("ON_2D") != -1)
				{
					go->MainCamera2D = renderManager->GetRender2D()->CurrentCamera;
					go->MainCamera3D = NULL;
					attachShaders2D(go->GetGLObject());
					convertGLTransform2DToComponent(go->GetGLObject(), go);
					go->IsAlive = true;
					continue;
				}
			}
		}

		if (word == "modelpath")
		{
			std::getline(objcontentStream, line);
			line.erase(
				std::remove_if(line.begin(),
					line.end(), isspace),
				line.end()
			);
			go->GetGLObject()->Model = new ModelLoader;
			go->GetGLObject()->Model->modelPath = line;
			continue;
		}

		if (word == "debug_aabb")
		{
			go->GetGLObject()->Model = new ModelLoader;
			go->GetGLObject()->Model->modelPath = word;
			go->GetGLObject()->Sections = 1;
			continue;
		}

		if (word == "debug_circle")
		{
			go->GetGLObject()->Model = new ModelLoader;
			go->GetGLObject()->Model->modelPath = word;
			go->GetGLObject()->Sections = 1;
			continue;
		}

		if (word == "scene")
		{
			go->GetGLObject()->Model = new ModelLoader;
			go->GetGLObject()->Model->modelPath = word;
			go->GetGLObject()->Sections = 1;
			continue;
		}

		if (word == "c")
		{
			std::getline(objcontentStream, line);
			line.erase(
				std::remove_if(line.begin(),
					line.end(), isspace),
				line.end()
			);
			attachComponents(line, go);
			continue;
		}

		if (word == "compprops")
		{
			std::getline(objcontentStream, line);
			std::getline(objcontentStream, line);
			go->InitializeAllComponents();
			go->SerializeAllComponents(&word, &objcontentStream);
			continue;
		}
	}

	if (go->GetName() != "UILoading")
	{
		if (!spawned)
			createImmediately(go);
		else
			createNextFrame(go);
	}
	return go;
}

void ResourceManager::Update2DGameObjects()
{

	std::vector<GameObject*>::iterator GO2D = gameObjects2D.begin();
	while (GO2D != gameObjects2D.end())
	{
		(*GO2D)->UpdateAllComponents();
		GO2D = GO2D + 1;
	}
}

void ResourceManager::Update3DDeferredObjects()
{
	std::vector<GameObject*>::iterator GO3D = gameObjects3D_Deferred.begin();
	while (GO3D != gameObjects3D_Deferred.end())
	{
		(*GO3D)->UpdateAllComponents();
		GO3D = GO3D + 1;
	}
}

void ResourceManager::Update3DGameObjects()
{
	
	if (gameObjects3D_Spawned.size() > 0)
	{
		for (auto spawned : gameObjects3D_Spawned)
		{
			if (!spawned->IsDeferred)
			{
				gameObjects3D.push_back(spawned);
			}
			else
			{
				// creating and spawning objects for deferred shading. Will be updated only in the last lighting pass
				// useful for light object creation only.
				gameObjects3D_Deferred.push_back(spawned);
			}
			gameObjects3D_Deferred;
			
		}
			
		gameObjects3D_Spawned.clear();
	}

	int bonecount = 0;
	int cubecount = 0;
	std::vector<GameObject*>::iterator GO3D = gameObjects3D.begin();

	/*size_t size_of_go = sizeof((*(*GO3D)));
	size_t vector_size = gameObjects3D.size() * size_of_go;
	std::cout << vector_size / 1000 << "kb";*/


	while (GO3D != gameObjects3D.end())
	//while (GO3D != gameObjects3D.begin() + 4)
	{
		if (!(*GO3D)->IsAlive)
		{
			/*if ((*GO3D)->NeedsDeletion)
			{

			}*/
			GO3D = GO3D + 1;
			continue;
		}

		if ((*GO3D)->IsDeferred)
		{
			GO3D = GO3D + 1;
			continue;
		}
			

		if ((*GO3D)->GetName() == "Cube")
		{
			cubecount++;
		}

		if ((*GO3D)->GetName() == "Boy01_Body_Geo")
		{
			cubecount++;
		}

		if ((*GO3D)->GetName() == "Hips")
		{
			cubecount++;
		}

		if ((*GO3D)->IsBone)
		{
			bonecount++;
		}
		
		(*GO3D)->UpdateAllComponents();
		GO3D = GO3D + 1;
	}

	//std::cout << "# of cubes = " << cubecount - 1 << std::endl;
	//std::cout << "# of bones = " << bonecount << std::endl;
}

void ResourceManager::createNextFrame(GameObject* go)
{
	if (go->GetGLObject()->Type == R2D)
	{
		gameObjects2D_Spawned.push_back(go);
	}
	if (go->GetGLObject()->Type == R3D)
	{
		if (gameObjects3D.size() == 0)
			this->ids.clear();
		gameObjects3D_Spawned.push_back(go);
	}
}


void ResourceManager::createImmediately(GameObject* go)
{
	if (go->GetGLObject()->Type == R2D)
	{
		gameObjects2D.push_back(go);
		gameObjects2D_map.insert(std::make_pair(go->GetName(), go));
	}
	if (go->GetGLObject()->Type == R3D)
	{
		if (gameObjects3D.size() == 0)
			this->ids.clear();
		gameObjects3D.push_back(go);
		gameObjects3D_map.insert(std::make_pair(go->GetName(), go));
	}
}

void ResourceManager::readModelData(GLObject* glo, std::string modelName, std::string objFile, GameObject* go)
{
	if (glo->Model)
	{
		glo->Model->Name = modelName;
		glo->Model->ParseOBJ(objFile);
	}
	else
	{
		// fbx loading has already data loaded
		// however, needs to set up initial transform data
		
		// This is temporary code, because I need to get the bind pose
		// with the animation data
		
		//if (go->IsBone)
		//{
		TransformComponent* goTrans = go->FindComponent<TransformComponent>();

		std::unordered_map<std::string, TransformData>::iterator transIt
			= glo->FModel->NodesMeshTransformData.find(go->GetName());

		if (transIt != glo->FModel->NodesMeshTransformData.end())
		{
			TransformData data;
			data = (*transIt).second;
			goTrans->SetPosition(data.translation);
			goTrans->SetRotation(data.rotation);
			//goTrans->SetScale(data.scale);
		}
		//}

		// part of fbx model reading process is to construct 
		// the hierarchy system need to get the parenthood info
		if (go->IsMesh)
		{
			// let's figure out what to do here later...
		}
		else
		{
			std::vector<ParentHood> parenthood;
			if (!go->IsBone)
				parenthood = glo->FModel->RelationsList;
			else
				parenthood = glo->FModel->GetAnimationObject()->RelationsList;

			std::vector<ParentHood>::iterator pit;
			std::string goName = go->GetName();

			// 1. set up all the children...
			pit = parenthood.begin();
			while (pit != parenthood.end())
			{
				std::map<std::string, GameObject*>::iterator gomap;
				if (goName == (*pit).nodeName)
				{
					std::vector<std::string> vec = (*pit).childrenName;
					std::vector<std::string>::iterator childit = vec.begin();
					while (childit != vec.end())
					{
						std::string childName = (*childit);
						//if (childName != "LeftFoot") // debug only
						//{
						gomap = gameObjects3D_map.find(childName); // NOTE: in this map, gameobjects are unique by name
						if (gomap != gameObjects3D_map.end())
						{
							go->SetChildren(gomap->second);
							gomap->second->SetParent(go); // 2. ...also the parent
						}
						//}

						childit++;
					}
					break;
				}
				pit++;
			}
		}

		int a = 0;

	}
}

void ResourceManager::readSingleVertexData(GLObject * glo)
{
	glo->Model->CreateSimpleVertex();
	glo->IsScenePoint = true;
}

void ResourceManager::readDebugLinesData(GLObject* glo)
{
	if(glo->Model->modelPath == "debug_aabb")
		glo->Model->Parse2DQuad();

	if (glo->Model->modelPath == "debug_circle")
		glo->Model->Parse2DCircle();
}

GameObject* ResourceManager::SpawnNewGameObject(std::string name)
{
	// create in CPU:
	GameObject* go = CreateOneGameObject(name);
	go->CompleteExtraSetup();

	ids.push_back(ids.size() + 1);
	go->SetId(ids.back());

	GLObject* glb = go->GetGLObject();
	std::string sppath = glb->Model->modelPath;
	readModelData(glb, name, sppath);

	// create in GPU:
	storeDataInGPU(go);
	std::vector<GameObject*> dummy; dummy.push_back(go);
	std::vector<GameObject*>::iterator dummyIt = dummy.begin();
	attachTexture(dummyIt);

	return go;
}

GameObject * ResourceManager::SpawnNewGameObjectFromArchetype(std::string name)
{
	// Testing archetype
	std::map<std::string, GameObject*>::iterator clonerIt;
	clonerIt = gameObjects3D_map.find(name);
	
	// create in CPU:
	GameObject* go = CreateOneGameObject(name, true);
	go->CompleteExtraSetup();

	ids.push_back(ids.size() + 1);
	go->SetId(ids.back());

	//readModelData(glb, name, sppath);
	GameObject* goSource = NULL;
	if (clonerIt != gameObjects3D_map.end())
		goSource = clonerIt->second;
	else
	{
		std::cout << "Trying to clone an inexistent game object!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
		return NULL;
	}
	GLObject* gloSource = goSource->GetGLObject();
	GLObject* glbCloned = go->GetGLObject();

	int positionAttributeIndex = 0, uvAttributeIndex = 1,
		normalAttributeIndex = 2, tangentAttributeIndex = 3;
	
	glbCloned->Sections = gloSource->Sections;
	glbCloned->ResizeVaosNumber(gloSource->Sections);
	glbCloned->VertexPerSection = gloSource->VertexPerSection;

	glbCloned->CreateBuffers();
	glbCloned->BuildVBOsFromOBJ(positionAttributeIndex, VERTEX, gloSource);
	glbCloned->BuildVBOsFromOBJ(uvAttributeIndex, UV, gloSource);
	glbCloned->BuildVBOsFromOBJ(normalAttributeIndex, NORMAL, gloSource);
	glbCloned->BuildVBOsFromOBJ(tangentAttributeIndex, TANGENT, gloSource);

	for (auto vertex : glbCloned->VertexPerSection)
	{
		glbCloned->SizeOfDataBySection.push_back(vertex);
	}

	std::vector<GameObject*> dummy; dummy.push_back(go);
	std::vector<GameObject*>::iterator goit = dummy.begin();
	glbCloned->Textures.resize(gloSource->Textures.size());
	for (size_t i = 0; i < gloSource->Textures.size(); i++)
	{
		std::string path = gloSource->Textures[i]->GetPath();
		size_t indx = path.find("ArtAssets");
		path = path.substr(indx, path.length());
		glbCloned->Textures[i] = new Texture(path);
		glbCloned->Textures[i]->Create();
	}

	glbCloned->ExtraTextures.resize(gloSource->ExtraTextures.size());
	for (size_t i = 0; i < gloSource->ExtraTextures.size(); i++)
	{
		std::string path = gloSource->ExtraTextures[i]->GetPath();
		size_t indx = path.find("ArtAssets");
		path = path.substr(indx, path.length());
		glbCloned->ExtraTextures[i] = new Texture(path);
		glbCloned->ExtraTextures[i]->Create();
	}


	glbCloned->Sections = gloSource->Sections;
	glbCloned->TexturesNames = gloSource->TexturesNames;
	glbCloned->TexturesMap = gloSource->TexturesMap;

	setTexIds(goit, glbCloned);
	cleanObjectModelMemory(glbCloned);

	return go;
}


GameObject* ResourceManager::SpawnNewGameObjectFromArchetypeForDeferredRendering(std::string name)
{
	// Testing archetype
	std::map<std::string, GameObject*>::iterator clonerIt;
	clonerIt = gameObjects3D_map.find(name);

	// create in CPU:
	GameObject* go = CreateOneGameObject(name, true, true);
	go->CompleteExtraSetup();
	ids.push_back(ids.size() + 1);
	go->SetId(ids.back());

	//readModelData(glb, name, sppath);
	GameObject* goSource = NULL;
	if (clonerIt != gameObjects3D_map.end())
		goSource = clonerIt->second;
	else
	{
		std::cout << "Trying to clone an inexistent game object!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
		return NULL;
	}
	GLObject* gloSource = goSource->GetGLObject();
	GLObject* glbCloned = go->GetGLObject();

	int positionAttributeIndex = 0, uvAttributeIndex = 1,
		normalAttributeIndex = 2, tangentAttributeIndex = 3;

	glbCloned->Sections = gloSource->Sections;
	glbCloned->ResizeVaosNumber(gloSource->Sections);
	glbCloned->VertexPerSection = gloSource->VertexPerSection;

	glbCloned->CreateBuffers();
	glbCloned->BuildVBOsFromOBJ(positionAttributeIndex, VERTEX, gloSource);
	glbCloned->BuildVBOsFromOBJ(uvAttributeIndex, UV, gloSource);
	glbCloned->BuildVBOsFromOBJ(normalAttributeIndex, NORMAL, gloSource);
	glbCloned->BuildVBOsFromOBJ(tangentAttributeIndex, TANGENT, gloSource);

	for (auto vertex : glbCloned->VertexPerSection)
	{
		glbCloned->SizeOfDataBySection.push_back(vertex);
	}

	std::vector<GameObject*> dummy; dummy.push_back(go);
	std::vector<GameObject*>::iterator goit = dummy.begin();
	glbCloned->Textures.resize(gloSource->Textures.size());
	for (size_t i = 0; i < gloSource->Textures.size(); i++)
	{
		std::string path = gloSource->Textures[i]->GetPath();
		size_t indx = path.find("ArtAssets");
		path = path.substr(indx, path.length());
		glbCloned->Textures[i] = new Texture(path);
		glbCloned->Textures[i]->Create();
	}

	glbCloned->ExtraTextures.resize(gloSource->ExtraTextures.size());
	for (size_t i = 0; i < gloSource->ExtraTextures.size(); i++)
	{
		std::string path = gloSource->ExtraTextures[i]->GetPath();
		size_t indx = path.find("ArtAssets");
		path = path.substr(indx, path.length());
		glbCloned->ExtraTextures[i] = new Texture(path);
		glbCloned->ExtraTextures[i]->Create();
	}


	glbCloned->Sections = gloSource->Sections;
	glbCloned->TexturesNames = gloSource->TexturesNames;
	glbCloned->TexturesMap = gloSource->TexturesMap;

	setTexIds(goit, glbCloned);
	cleanObjectModelMemory(glbCloned);

	// creating and spawning objects for deferred shading. Will be updated only in the last lighting pass
	// useful for light object creation only.
	//gameObjects3D_Deferred.push_back(go);

	return go;
}


void ResourceManager::storeDataInGPU(GameObject* go)
{
	GLObject* glo = go->GetGLObject();
	int positionAttributeIndex = 0, uvAttributeIndex = 1,
		normalAttributeIndex = 2, tangentAttributeIndex = 3,
		boneIdAttributeIndex = 4, boneWeightsAttributeIndex = 5;

	glo->Sections = (glo->Model) ? glo->Model->GetSectionsToDraw() : 1;
	glo->ResizeVaosNumber(glo->Sections);

	if (glo->Model)
		glo->VertexPerSection = glo->Model->GetVertexPerSection();
	else
	{
		// for fbx objects, we will use 1 only section for now
		// until I figure out how to get many textures in one only mesh
		std::vector<int> dummy(1);
		
		// GetBoneMeshData from AnimationData
		// validate if is bone, get this,
		// otherwise, GetFBXMeshData
		if (go->IsMesh)
		{
			dummy[0] = glo->GetAnimationMeshData(VERTEX).numberOfData;
		}
		else
		{
			if (!go->IsBone)
				dummy[0] = glo->GetFBXMeshData(VERTEX).numberOfData;
			else
				dummy[0] = glo->GetBoneMeshData(VERTEX).numberOfData;
		}

		glo->VertexPerSection = dummy;
	}
	// retrieve the game object id and send to the GPU
	//if(go->IsBone)
	//	glo->Oid = go->GetId();


	glo->CreateBuffers();
	glo->BuildVBOsFromOBJ(positionAttributeIndex, VERTEX);
	if (!glo->IsScenePoint) // if object is a simple scene point, only vertex data is needed
	{
		glo->BuildVBOsFromOBJ(uvAttributeIndex, UV);
		glo->BuildVBOsFromOBJ(normalAttributeIndex, NORMAL);
		if (go->IsMesh)
		{
			glo->BuildVBOsFromOBJ(boneIdAttributeIndex, BONEID);
			glo->BuildVBOsFromOBJ(boneWeightsAttributeIndex, BONEWEIGHT);
		}
	}
	
	//glo->BuildVBOsFromOBJ(tangentAttributeIndex, TANGENT);
	for (auto vertex : glo->VertexPerSection)
		glo->SizeOfDataBySection.push_back(vertex);
	

}

void ResourceManager::storeDebugDataInGPU(GLObject * glo)
{
	glo->ResizeVaosNumber(1);
	glo->CreateBuffers();
	glo->VertexPerSection = glo->Model->GetVertexPerSection();
	glo->BuildVBOsFromOBJ(0, VERTEX);
	glo->SizeOfDataBySection.push_back(glo->VertexPerSection[0]);
	delete glo->Model;
	glo->Model = NULL;
}

void ResourceManager::attachShaders3D(GLObject* glo)
{
	glo->Shader = shader;
	glo->Shader->Disable2D();
	glo->Type = R3D;
}

void ResourceManager::attachShaders2D(GLObject* glo)
{
	glo->Shader = shader;
	glo->Shader->Enable2D();
	glo->Type = R2D;
}

void ResourceManager::convertGLTransform3DToComponent(GLObject* glo, GameObject* go)
{
	glo->Transform = new Transform;
	glo->Transform->Camera = go->MainCamera3D;
	//glo->Transform->SetTransformMode(glo->Type);
	
	TransformComponent* transform = new TransformComponent(glo);
	go->AddComponent(transform);
}

void ResourceManager::convertGLTransform2DToComponent(GLObject* glo, GameObject* go)
{
	glo->Transform = new Transform;
	glo->Transform->Camera2D = go->MainCamera2D;

	TransformComponent* transform = new TransformComponent(glo);
	go->AddComponent(transform);
}

void ResourceManager::setTexIds(std::vector<GameObject*>::iterator goit, GLObject* glo)
{
	this->texIds;

	std::vector<GLuint> texIds;

	std::vector<Texture*>::iterator it = glo->Textures.begin();
	while (it != glo->Textures.end())
	{
		if (!*it)
			printf("Error: Trying to attach a NULL texture");

		if ((*it)->GetFont() == NULL)
		{
			GLuint id = (*it)->Create();
			texIds.push_back(id);
		}

		else
		{
			(*it)->CreateFromText("1", { 255, 0, 0 });
		}


		it = it + 1;
	}

	if (glo->ExtraTextures.size() > 0)
	{
		std::vector<Texture*>::iterator it = glo->ExtraTextures.begin();
		while (it != glo->ExtraTextures.end())
		{
			GLuint id = (*it)->Create();
			texIds.push_back(id);
			it = it + 1;
		}
	}


	GameObject* lastGO = NULL;
	GLuint lastTexId = 0;

	if ((*goit)->GetId() > 1)
	{
		//std::vector<GameObject*>::iterator tmpIt = goit - 1;
		lastTexId = this->texIds.back();
	}

	for (size_t i = 0; i < glo->TexturesNames.size(); i++)
	{

		std::string name = glo->TexturesNames[i];
		std::unordered_map<std::string, int>::const_iterator it = glo->TexturesMap.find(name);
		if (it != glo->TexturesMap.end())
		{
			GLuint actualId = lastTexId + it->second;
			glo->TexturesIdsToDraw.push_back(actualId);
		}
	}
	this->texIds.push_back((*goit)->GetGLObject()->Textures.back()->GetId());
	if (glo->ExtraTextures.size() > 0)
		this->texIds.push_back((*goit)->GetGLObject()->ExtraTextures.back()->GetId());
}

void ResourceManager::cleanObjectModelMemory(GLObject* glo)
{
	delete glo->Model;
	glo->Model = NULL;
}

void ResourceManager::attachTexture(std::vector<GameObject*>::iterator goit)
{
	GLObject* glo = (*goit)->GetGLObject();

	if (glo->Model)
	{
		glo->Textures = glo->Model->getModelTextures();

		glo->ExtraTextures = glo->Model->ExtraTextures;

		glo->Sections = glo->Model->GetSectionsToDraw();

		std::vector<std::string> tmp1 = glo->Model->GetTextureNames();
		for (auto i : tmp1)
		{
			glo->TexturesNames.push_back(i);
		}

		std::unordered_map<std::string, int> tmp2 = glo->Model->GetTextureMap();
		for (auto i : tmp2)
		{
			glo->TexturesMap.insert(i);
		}
		/*glo->TexturesNames = glo->Model->GetTextureNames();
		glo->TexturesMap = glo->Model->GetTextureMap();*/

		setTexIds(goit, glo);
		cleanObjectModelMemory(glo);
	}
	else
	{
		if ((*goit)->IsMesh)
		{
			// add the procedure to attach the mesh texture found
			Texture* texture = glo->FModel->GetAnimationObject()->InstanceAnimMeshTexture((*goit)->GetName());
			glo->Textures.push_back(texture);
			glo->Sections = 1;
			setTexIds(goit, glo);

			int a = 0;
			a++;
		}
		else
		{
			if (!(*goit)->IsBone)
			{
				glo->Textures = glo->FModel->GetMaterialsTextures((*goit)->GetName());
				glo->Sections = 1;
				setTexIds(goit, glo);
			}
			else
			{
				Texture* texture = glo->FModel->GetAnimationObject()->InstanceDefeultTexture();
				glo->Textures.push_back(texture);
				glo->Sections = 1;
				setTexIds(goit, glo);

			}
		}
	}
}


GameObject* ResourceManager::GetGameObjectByName(std::string name)
{
	auto ptr = gameObjects3D_map.find(name);
	if (ptr != gameObjects3D_map.end())
		return ptr->second;
	return NULL;
}

