#pragma once
#include "Camera3D.h"
#include "Camera2D.h"
#include "GameObject.h"
#include "Quaternion.h"
#include "VQS.h"

#include <glm.hpp>

enum TRANSFORM_TYPE
{
	LOCAL,
	WORLD,

	MAX_TRANS
};

class Transform
{

private:
	Quaternion oldQuat;
	int initialState = 0;
	float initialTransform = 0.0f;
	int isLine = 0;
	int isMesh = 0;

public:



	glm::vec3 Position;
	glm::vec3 RotationAngle;
	glm::vec3 Size;

	glm::mat4 ModelTransform;
	glm::mat4 ModelToWorld;
	VQS VQS_ModelToWorld;

	glm::vec3 ParentPosition;
	glm::vec3 ParentRotation;
	glm::vec3 ParentScale;

	glm::vec3 ConnectorsPosition;
	glm::vec3 ConnectorsRotation;
	glm::vec3 ConnectosScale;
	glm::vec3 ConnectorsParentPosition;
	glm::vec3 ConnectorsParentRotation;
	glm::vec3 ConnectosParentScale;

	glm::mat4 TranslateForBoneMatrix;
	glm::mat4 RotationForBoneMatrix;
	glm::mat4 ScaleForBoneMatrix;

	glm::vec3 VQS_TranslatorForBone;
	Quaternion VQS_RotatorForBone;
	Quaternion VQS_RotatorForBoneIK;
	float VQS_ScalerForBone;

	glm::mat4 ModelToWorldForBone;
	VQS VQS_ModelToWorldForBone;

	glm::mat4 TranslateForAnimMeshMatrix;
	glm::mat4 RotationForAnimMeshMatrix;
	glm::mat4 ScaleForAnimMeshMatrix;

	glm::vec3 VQS_TranslatorForAnimMesh;
	Quaternion VQS_RotatorForAnimMesh;
	float VQS_ScalerForAnimMesh;

	glm::mat4 ModelToWorldForAnimMesh;
	VQS VQS_ModelToWorldForAnimMesh;

	//int DeformID = 1000;
	//std::vector<glm::mat4> DeformMatrices;
	std::map<std::string, glm::mat4> DeformMatrices;

	TRANSFORM_TYPE TransType;

	Camera3D* Camera;
	Camera2D* Camera2D;

	Transform();
	~Transform();

	virtual void Init() {}
	virtual void Update(GameObject** go);

	void SetTransformMode(GLObject* globject);
	
};

