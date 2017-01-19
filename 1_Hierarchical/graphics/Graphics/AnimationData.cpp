#include "AnimationData.h"

#include <iostream>



AnimationData::AnimationData()
{
}


AnimationData::~AnimationData()
{
}


int AnimationData::GetAnimationStackCount()
{
	// 1. get number of animaton stacks
	int numStacks = scene->GetSrcObjectCount<FbxAnimStack>();
	return numStacks;
}

void AnimationData::colllectAnimationStacks()
{
	scene->FillAnimStackNameArray(animStackNamesList);
	// 2. retrieve every single animation stack (if more than 1)

	std::vector<FbxAnimStack*> animStackList;
	for (int i = 0; i < animStackNamesList.Size(); i++)
	{
		FbxAnimStack* animationStack =
			scene->FindMember<FbxAnimStack>(animStackNamesList[i]->Buffer());
		if (animationStack == NULL)
		{
			std::cout << "Animation stack #" << i << "was NULL" << std::endl;
			continue;
		}
		animStackList.push_back(animationStack);
	}
	defaultAnimStack = animStackList[0];

	//
	
}

void AnimationData::setDefaultAnimationLayer()
{
	// 3.  Retrieve the number of animation layers from this animation stack.
	int animLayers = defaultAnimStack->GetSrcObjectCount<FbxAnimLayer>();
	// 4. Retrieve the animation layer for this animation stack
	defaultAnimLayer = defaultAnimStack->GetMember<FbxAnimLayer>(0);
}

void AnimationData::setBindPose()
{
	// 5. Retrieve the bind pose (Assume only one)
	// but prepare code for when there exists
	// more than one pose.
	const int poseCount = scene->GetPoseCount();
	std::vector<FbxPose*> poseArray(poseCount);
	for (int i = 0; i < poseCount; i++)
	{
		poseArray[i] = scene->GetPose(i);
	}
	if(poseArray.size() > 0)
		bindPose = poseArray[0];
}

bool AnimationData::ReadBones()
{
	colllectAnimationStacks();
	if (animStackNamesList.Size() <= 0)
		return false;

	setDefaultAnimationLayer();
	if (!defaultAnimLayer)
		return false;

	setBindPose();
	if (!bindPose)
		return false; // check this... does every animation have a bind pose?

	FbxNode* parentNode = scene->GetRootNode();
	
	if (!parentNode)
		throw std::exception("Something went wrong! - Root node is NULL");
	traverseBones(parentNode);

	return true;
}

void AnimationData::traverseBones(FbxNode* bone)
{
	// debug
	const char* boneName = bone->GetName();
	size_t numChildren = static_cast<size_t>(bone->GetChildCount());
	ParentHood parentHood;
	parentHood.nodeName = boneName;

	for (size_t i = 0; i < numChildren; i++)
	{
		const char* n = bone->GetChild(i)->GetName();
		parentHood.childrenName.push_back(n);
	}
	RelationsList.push_back(parentHood);
	

	//To get the number of children of the root node
	size_t parentChildrenNumber = static_cast<size_t>(bone->GetChildCount());
	FbxNode* childNode = NULL;
	for (size_t i = 0; i < parentChildrenNumber; i++)
	{
		childNode = bone->GetChild(i);

		const char* childNodeName = childNode->GetName();
		size_t childChildrenNumber = static_cast<size_t>(childNode->GetChildCount());

		std::string str_name(childNodeName);
		/*bool invalid1 = (str_name == "headusOBJexport");
		bool invalid2 = (str_name == "Point01");
		bool invalid3 = (str_name == "Point02");
		bool invalid4 = (str_name == "Bip01 Footsteps");*/

		/*bool invalid1 = (str_name == "Boy01_Body_Geo");
		bool invalid2 = (str_name == "Boy01_Brows_Geo");
		bool invalid3 = (str_name == "Boy01_Eyes_Geo");
		bool invalid4 = (str_name == "h_Geo");*/

		FbxNodeAttribute* lNodeAttribute = childNode->GetNodeAttribute();
		// TODO: uncomment to implement the skinning deformation
		
		//// for meshes:
		//if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		//{
		//	Mesh animationMesh;
		//	animationMesh.SetScene(scene);
		//	animationMesh.SetAnimStack(defaultAnimStack);
		//	animationMesh.SetAnimationLayer(defaultAnimLayer);
		//	animationMesh.SetNode(childNode);
		//	animationMesh.Initialize();
		//	animationMesh.StoreData();
		//	AnimationMeshes.push_back(animationMesh);
		//}

		// for skeleton:
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			FbxSkeleton* skeleton = (FbxSkeleton*)childNode->GetNodeAttribute();
			FbxSkeleton::EType boneType = skeleton->GetSkeletonType();
			bool createBone = false;
			switch (boneType)
			{
			case fbxsdk::FbxSkeleton::eRoot:
				createBone = true;
				break;
			case fbxsdk::FbxSkeleton::eLimb:
				createBone = true;
				break;
			case fbxsdk::FbxSkeleton::eLimbNode:
				createBone = true;
				break;
			case fbxsdk::FbxSkeleton::eEffector:
				createBone = true;
				break;
			default:
				createBone = false;
				break;
			}

			if (createBone)
			{
				Bone boneData;
				boneData.SetScene(scene);
				boneData.SetAnimStack(defaultAnimStack);
				boneData.StoreData(childNode, defaultAnimLayer, bindPose);
				BonesMapByName.insert(std::make_pair(childNodeName, boneData));
				BonesListInDepthFirst.push_back(boneData);
			}
		}
		
		traverseBones(childNode);

	}
	

	





}

Texture* AnimationData::InstanceAnimMeshTexture(std::string meshName)
{
	Texture* texture = NULL;

	std::vector<Mesh>::iterator animMeshIt = AnimationMeshes.begin();
	while (animMeshIt != AnimationMeshes.end())
	{
		std::string mName = animMeshIt->GetName();
		if (mName == meshName)
			texture = animMeshIt->GetMeshMaterials()[0].textures[0];
		animMeshIt++;
	}
	return texture;
}

Texture* AnimationData::InstanceDefeultTexture()
{
	//std::string relativePath(SDL_GetBasePath());
	std::string textureFile = "ArtAssets\\DefaultColorWhite.png";
	std::string textureName = "DefaultColorWhite";

	Texture* texture = new Texture(textureFile);
	return texture;
}
