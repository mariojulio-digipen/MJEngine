#include "Bone.h"
#include <SDL.h>
#include <gtc\matrix_transform.hpp>
#include <gtx\matrix_decompose.hpp>
#include <gtc\type_ptr.hpp>
#include <gtx\euler_angles.hpp>
#include <glm.hpp>
#include "Quaternion.h"
#include "VQS.h"

#include "FrameRateController.h"

#include <iostream>
extern FrameRateController* frameRateController;

void convertToGLM(glm::mat4* dstMatrix, FbxAMatrix srcAMatrix);
void Bone::storeModelWorldMatrixIfBindedToAPose(FbxNode* node, FbxPose* pose)
{
	globalPosition;
	
	std::string thisNodeName = node->GetName();

	int nodeIdx = pose->Find(node);
	if (nodeIdx > -1)
	{
		if (pose->IsBindPose() || !pose->IsLocalMatrix(nodeIdx)) //if global
		{
			FbxAMatrix poseMatrix;
			FbxMatrix matrixtmp = pose->GetMatrix(nodeIdx);
			memcpy((double*)poseMatrix, (double*)matrixtmp, sizeof(matrixtmp.mData));
			globalPosition = poseMatrix;
		}
		else // if local, tranform to global
		{
			FbxAMatrix tmpParentGlobalPos;
			FbxNode* parentNode = node->GetParent();
			if (parentNode)
			{
				int nodeIdx = pose->Find(parentNode);
				FbxAMatrix poseMatrix;
				FbxMatrix matrixtmp = pose->GetMatrix(nodeIdx);
				memcpy((double*)poseMatrix, (double*)matrixtmp, sizeof(matrixtmp.mData));

				tmpParentGlobalPos = poseMatrix;
			}

			FbxAMatrix poseMatrix;
			FbxMatrix matrixtmp = pose->GetMatrix(nodeIdx);
			memcpy((double*)poseMatrix, (double*)matrixtmp, sizeof(matrixtmp.mData));
			FbxAMatrix localpos = poseMatrix;
			globalPosition = tmpParentGlobalPos * localpos;
		}
		glm::dvec4 c0 = glm::make_vec4((double*)globalPosition.GetColumn(0).Buffer());
		glm::dvec4 c1 = glm::make_vec4((double*)globalPosition.GetColumn(1).Buffer());
		glm::dvec4 c2 = glm::make_vec4((double*)globalPosition.GetColumn(2).Buffer());
		glm::dvec4 c3 = glm::make_vec4((double*)globalPosition.GetColumn(3).Buffer());
		glm::mat4 tmp = glm::mat4(static_cast<glm::vec4>(c0), static_cast<glm::vec4>(c1),
			static_cast<glm::vec4>(c2), static_cast<glm::vec4>(c3));
		BindPoseModelWorldMatrix = tmp;
		BindPoseModelWorldMatrix = glm::transpose(BindPoseModelWorldMatrix);
		IsLInkedToBindPose = true;
	}
	else
	{
		glm::vec4 c0(-1.0f, -1.0f, -1.0f, -1.0f);
		glm::vec4 c1(-1.0f, -1.0f, -1.0f, -1.0f);
		glm::vec4 c2(-1.0f, -1.0f, -1.0f, -1.0f);
		glm::vec4 c3(-1.0f, -1.0f, -1.0f, -1.0f);
		glm::mat4 tmp = glm::mat4(c0, c1, c2, c3);
		BindPoseModelWorldMatrix = tmp;
		IsLInkedToBindPose = false;
	}
}

void Bone::storeName(FbxNode* node)
{
	Name = node->GetName();
}

// This function retrieves the local transform of the default 
// position in the start key frame. This is the starting
// point of the cyclic animation
void Bone::storeTransform(FbxNode* node)
{
	FbxAMatrix matrixGeo;
	matrixGeo.SetIdentity();

	const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);
	matrixGeo.SetT(lT);
	matrixGeo.SetR(lR);
	matrixGeo.SetS(lS);


	// local matrix
	FbxAMatrix localMatrix = 
		node->EvaluateLocalTransform();
	
	FbxAMatrix local = localMatrix*matrixGeo;
	const FbxVector4 translation = local.GetT();
	const FbxQuaternion rotator = local.GetQ();
	const FbxVector4 scaling = local.GetS();

	// convert to VQS system
	convertKeyFrameToVQS(&Transform.VQS_ModelToWorld, &translation, &rotator, &scaling);

}



// First, the Animation Take is retrieved so that the 
// animation Timelapse can be used. From the Timelapse, 
// the start and stop keyframes are stored in temporary 
// variables. Then, another variable stores the currentFrameTime
// as the start. From that point, a loop iterates over 
// every keyframe, extracting their local transform matrix from 
// which the VQS system is generated and pushed into a 
// KeyFrames_temp vector(array). This process is repeated until 
// the current frame time is greater or equal to the stop frame 
// time. Finally, another loop iterates the KeyFrames_temp vector 
// to generate a final KeyFrames vector, by interpolating every 
// two pairs of keyframes’ VQS systems, inserting 49 (divide - 1) new VQS 
// systems in between the pair.
void Bone::storeKeyFrame(FbxNode* node)
{
	FbxTime frameTime;
	FbxTime currentTime;
	FbxTime start;
	FbxTime stop;

	FbxString stackname = animStack->GetName();
	FbxTakeInfo* currentTakeInfo = scene->GetTakeInfo(stackname);
	start = currentTakeInfo->mLocalTimeSpan.GetStart();
	stop = currentTakeInfo->mLocalTimeSpan.GetStop();

	frameTime.SetTime(0, 0, 0, 1, 0, scene->GetGlobalSettings().GetTimeMode());

	currentTime = start;


	FbxAMatrix matrixGeo;
	matrixGeo.SetIdentity();
	const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);
	matrixGeo.SetT(lT);
	matrixGeo.SetR(lR);
	matrixGeo.SetS(lS);
	glm::mat4 tmpmatrix;

	BoneKeyFrame keyframe;
	std::vector<BoneKeyFrame> KeyFrames_temp;
	int i = 0;
	while (currentTime <= stop)
	{
		// adds the keyframe index
		keyframe.index = i; 
		// adds the keyframe internal number 
		//keyframe.time = static_cast<long long>(currentTime.Get());

		// Evaluate transform matrix, current time starts from the animation's timelapse start point
		FbxAMatrix matrix;
		FbxAMatrix mat;
		std::string name = node->GetName();

		// Use root node instead the name
		
		FbxString parentName = node->GetParent()->GetName();
		FbxNode* grandParent = node->GetParent()->GetParent();
		//FbxString grandParentName = node->GetParent()->GetParent()->GetName();

		int childCount = node->GetChildCount();

		if (grandParent == NULL)
			Type = BONE_TYPE::ROOT;
		else
			Type = BONE_TYPE::LIMB;
		

		int a = 0;
		if (name == "LeftShoulder")
			a++;

		//if(Type == BONE_TYPE::ROOT)
		//	matrix = node->EvaluateGlobalTransform(currentTime);//node->EvaluateLocalTransform(currentTime);
		//else
		//	matrix = node->EvaluateLocalTransform(currentTime);//node->EvaluateLocalTransform(currentTime);
		//
		matrix = node->EvaluateLocalTransform(currentTime);
		mat = matrix*matrixGeo;

		// convert to VQS system
		const FbxVector4 translation = mat.GetT();
		const FbxQuaternion rotator = mat.GetQ();
		const FbxVector4 scaling = mat.GetS();
		VQS VQS_KeyFrameMatrix;
		convertKeyFrameToVQS(&VQS_KeyFrameMatrix, &translation, &rotator, &scaling);
		keyframe.VQS_Transform = VQS_KeyFrameMatrix;
		keyframe.time = static_cast<long double>(currentTime.Get());

		currentTime += frameTime;
		//KeyFrames_temp.push_back(keyframe);
		KeyFrames.push_back(keyframe);
		i++;
	}

	// normalize times
	std::vector<long double> debugVec;
	std::vector<BoneKeyFrame>::iterator boneIt = KeyFrames.begin();
	std::vector<BoneKeyFrame>::iterator boneMaxIt = KeyFrames.end() - 1;
	while (boneIt != KeyFrames.end())
	{
		boneIt->time = boneIt->time / boneMaxIt->time;
		debugVec.push_back(boneIt->time);
		boneIt++;
	}
	return;

	// interpolate vqs's and push back into the final KeyFrames vector
	//size_t frame_num = KeyFrames_temp.size();
	//for (size_t i = 0; i < frame_num - 1; i++)
	//{
	//	// do interpolation here...
	//	VQS keyframe_0 = KeyFrames_temp[i].VQS_Transform;
	//	VQS keyframe_1 = KeyFrames_temp[i + 1].VQS_Transform;

	//	float divide = 50.0f;
	//	float jump = 1.0f / divide;
	//	KeyFrames.push_back(KeyFrames_temp[i]);
	//	for (float u = jump; u < 1; u += jump)
	//	{
	//		VQS newvqs_u = VQS::Interpolate(keyframe_0, keyframe_1, u);
	//		BoneKeyFrame newkeyframe_u; newkeyframe_u.VQS_Transform = newvqs_u;
	//		newkeyframe_u.index = KeyFrames_temp[i].index;
	//		KeyFrames.push_back(newkeyframe_u);
	//	}

	//	if (i == frame_num - 2)
	//		KeyFrames.push_back(KeyFrames_temp[i + 1]);
	//}
	
	
	

}

// The VQS system is generated from the translate, quaternion 
// rotation and scale of the local transform matrix of a node
void Bone::convertKeyFrameToVQS(VQS* vqs, const FbxVector4* translator, const FbxQuaternion* rotator, const FbxVector4* scaling)
{
	glm::vec3 V = glm::vec3(static_cast<float>((*translator)[0]),
		static_cast<float>((*translator)[1]), static_cast<float>((*translator)[2]));
	Quaternion Q = Quaternion(static_cast<float>((*rotator)[3]),
		glm::vec3(static_cast<float>((*rotator)[0]), static_cast<float>((*rotator)[1]), static_cast<float>((*rotator)[2])));
	float S = static_cast<float>((*scaling)[0]);
	vqs->Translator = Quaternion(0.0f, V);
	vqs->Rotator = Q; vqs->Rotator.Normalize();
	vqs->ScaleUniform = S;
}

void Bone::storeMeshData()
{//0.0295f
	static const float vertx[] = {
		// Back face
		-1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		// Front face
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		// Left face
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,

		// Right face
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		// Bottom face
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,

		// Top face
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f
	};
	std::vector<float> coord(vertx, vertx + sizeof(vertx) / sizeof(vertx[0]));
	MeshData.coordData = coord;

	static const float normals[] = {
		// Back face
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		// Front face
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		// Left face
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//Right face
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		// Bottom face
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		// Top face
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	std::vector<float> norms(normals, normals + sizeof(normals) / sizeof(normals[0]));
	MeshData.normalData = norms;

	static const float uvs[] = {
		// Back face
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		// Front face
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		// Left face
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		// Right face
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		// Bottom face
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		// Top face
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f
	};
	std::vector<float> uv(uvs, uvs + sizeof(uvs) / sizeof(uvs[0]));
	MeshData.textureData = uv;

}

void Bone::storeParentName(FbxNode* node)
{
	FbxNode* parentNode = node->GetParent();
	if(parentNode)
		ParentName = parentNode->GetName();
}

void Bone::storeChildrenName(FbxNode* node)
{
	size_t numChildren = static_cast<size_t>(node->GetChildCount());
	for (size_t i = 0; i < numChildren; i++)
	{
		const char* n = node->GetChild(i)->GetName();
		ChildrenNames.push_back(n);
	}
}

void Bone::storeType(FbxNode * node)
{
	FbxNodeAttribute* boneAttData = node->GetNodeAttribute();
	if (boneAttData)
	{
		FbxSkeleton* skeleton = (FbxSkeleton*)node->GetNodeAttribute();
		FbxSkeleton::EType boneType = skeleton->GetSkeletonType();
		if (boneType == FbxSkeleton::eLimbNode)
			Type = BONE_TYPE::LIMB;
		if (boneType == FbxSkeleton::eRoot)
			Type = BONE_TYPE::ROOT;
	}
}

void Bone::storeAnimationCurve(FbxNode * node, FbxAnimLayer * animLayer)
{
	FbxAnimCurve* animCurve = NULL;

	//animCurve = node->GetCurve
	
}

void Bone::StoreData(FbxNode* node, FbxAnimLayer* animLayer, FbxPose* pose)
{
	// Set the bone name
	storeName(node);

	// Generate a simple white cube data
	storeMeshData();

	// Store parent name to match parenthood later
	storeParentName(node);

	// Store names of all the children
	storeChildrenName(node);

	// Get the global position
	storeModelWorldMatrixIfBindedToAPose(node, pose);

	// store skeleton type:
	storeType(node);

	// Get transformation data
	storeTransform(node);

	// Store Animation Keyframes data
	storeKeyFrame(node);

}

void convertToGLM(glm::mat4* dstMatrix, FbxAMatrix srcAMatrix)
{
	(*dstMatrix)[0][0] = static_cast<float>(srcAMatrix.mData[0][0]);
	(*dstMatrix)[0][1] = static_cast<float>(srcAMatrix.mData[0][1]);
	(*dstMatrix)[0][2] = static_cast<float>(srcAMatrix.mData[0][2]);
	(*dstMatrix)[0][3] = static_cast<float>(srcAMatrix.mData[0][3]);

	(*dstMatrix)[1][0] = static_cast<float>(srcAMatrix.mData[1][0]);
	(*dstMatrix)[1][1] = static_cast<float>(srcAMatrix.mData[1][1]);
	(*dstMatrix)[1][2] = static_cast<float>(srcAMatrix.mData[1][2]);
	(*dstMatrix)[1][3] = static_cast<float>(srcAMatrix.mData[1][3]);

	(*dstMatrix)[2][0] = static_cast<float>(srcAMatrix.mData[2][0]);
	(*dstMatrix)[2][1] = static_cast<float>(srcAMatrix.mData[2][1]);
	(*dstMatrix)[2][2] = static_cast<float>(srcAMatrix.mData[2][2]);
	(*dstMatrix)[2][3] = static_cast<float>(srcAMatrix.mData[2][3]);

	(*dstMatrix)[3][0] = static_cast<float>(srcAMatrix.mData[3][0]);
	(*dstMatrix)[3][1] = static_cast<float>(srcAMatrix.mData[3][1]);
	(*dstMatrix)[3][2] = static_cast<float>(srcAMatrix.mData[3][2]);
	(*dstMatrix)[3][3] = static_cast<float>(srcAMatrix.mData[3][3]);
}