#pragma once
#include <glm.hpp>
#include <vector>
#include <fbxsdk.h>

#include "VQS.h"

enum BONE_TYPE
{
	ROOT,
	LIMB,

	BONE_MAX
};

struct BoneTransform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4 ModelToWorld;
	VQS VQS_ModelToWorld;


};


struct BoneKeyFrame
{
	int index;
	long double time;
	//glm::mat4 Matrix;
	VQS VQS_Transform;
};

struct BoneMesh
{
	std::vector<float> coordData;
	std::vector<float> textureData;
	std::vector<float> normalData;
	std::vector<float> tangentData;
};


class Bone
{
private:

	FbxAMatrix globalPosition;
	FbxAMatrix localPosition;

	FbxScene* scene;
	FbxAnimStack* animStack;
	
	//helper functions
	void storeName(FbxNode* node);
	void storeTransform(FbxNode* node);
	void storeMeshData();
	void storeParentName(FbxNode* node);
	void storeChildrenName(FbxNode* node);
	void storeModelWorldMatrixIfBindedToAPose(FbxNode* node, FbxPose* pose);
	void storeType(FbxNode* node);
	void storeAnimationCurve(FbxNode* node, FbxAnimLayer* animLayer); // not used anymore
	void storeKeyFrame(FbxNode* node);
	void convertKeyFrameToVQS(VQS* vqs, const FbxVector4* translator, const FbxQuaternion* rotator, const FbxVector4* scaling);

public:

	std::string Name;
	BoneTransform Transform;
	BoneMesh MeshData;
	BoneMesh ConnectorData;
	std::string ParentName;
	std::vector<std::string> ChildrenNames;
	bool BindPoseActive;
	bool IsLInkedToBindPose;
	glm::mat4 BindPoseModelWorldMatrix; 
	BONE_TYPE Type;
	
	std::vector<BoneKeyFrame> KeyFrames;
	

	Bone() : IsLInkedToBindPose(false), BindPoseActive(false){}
	~Bone() {}

	void StoreData(FbxNode* node, FbxAnimLayer* animLayer, FbxPose* pose);
	void SetScene(FbxScene* pScene) { this->scene = pScene; }
	void SetAnimStack(FbxAnimStack* pStack) { this->animStack = pStack; }
};
