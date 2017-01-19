#pragma once
#include <glm.hpp>
#include <fbxsdk.h>
#include <vector>
#include <map>

#include "Bone.h"
#include "Mesh.h"
#include "MJFBXImporter.h"


class AnimationData
{

private:

	
	
	FbxManager* sdkManager;
	FbxScene* scene;

	FbxArray<FbxString*> animStackNamesList;
	FbxAnimLayer* defaultAnimLayer;
	FbxAnimStack* defaultAnimStack;
	FbxPose* bindPose;

	void colllectAnimationStacks();
	void setDefaultAnimationLayer();
	void setBindPose();
	void traverseBones(FbxNode* bone);

	


public:

	std::vector<ParentHood> RelationsList;
	std::unordered_map<std::string, Bone> BonesMapByName;
	std::vector<Bone> BonesListInDepthFirst;

	std::vector<Mesh> AnimationMeshes;


	AnimationData();
	AnimationData(FbxManager* sdkmanager, FbxScene* pscene) : 
		sdkManager(sdkmanager), scene(pscene){}
	~AnimationData();


	int GetAnimationStackCount();
	bool ReadBones();
	Texture* InstanceDefeultTexture();
	Texture* InstanceAnimMeshTexture(std::string meshName);


};

