#include "Transform.h"
#include "Shader.h"
#include "GLObject.h"
#include "UIManager.h"
#include "Quaternion.h"
#include "FrameRateController.h"

#include <gtc\matrix_transform.hpp>
#include <gtx\matrix_decompose.hpp>
#include <gtc\type_ptr.hpp>
#include <gtx\euler_angles.hpp>
#include <glm.hpp>

#include "ResourceManager.h"
#include "TransformComponent.h"

extern ResourceManager* resourceManager;
extern FrameRateController* frameRateController;

extern Shader* shader;
extern Shader* gbufferShader;
extern Shader* simpleShader;
extern UIManager* uiManager;

Transform::Transform() : RotationAngle(glm::vec3(0.0f, 0.0f, 0.0f))/*, VQS_ScalerForBone(1.0f)*/
{
	TranslateForBoneMatrix = glm::mat4(1.0f);
	RotationForBoneMatrix = glm::mat4(1.0f);
	ScaleForBoneMatrix = glm::mat4(1.0f);

	
}


Transform::~Transform()
{
}

void Transform::Update(GameObject** go)
{
	if ((*go)->GetGLObject()->Type == R3D)
	{
		GameObject* parent = (*go)->GetParent();

		// Matrix system
		/*glm::mat4 translate;
		glm::mat4 rotate;
		glm::mat4 scale;

		
		translate = glm::translate(glm::mat4(1.0f), Position);
		rotate = glm::eulerAngleXYZ
		(
			glm::radians(RotationAngle.x),
			glm::radians(RotationAngle.y),
			glm::radians(RotationAngle.z)
		);
		scale = glm::scale(glm::mat4(1.0f), Size);*/


		/*if ((*go)->GetName() == "Hips")
		{
			int a = 0; a++;
		}*/



		// the translate vector
		glm::vec3 translator = Position;

		// the rotation quaternion
		// computed using 
		Quaternion rotator = Quaternion::QuatFromAngles(glm::vec3(
			glm::radians(RotationAngle.x),
			glm::radians(RotationAngle.y),
			glm::radians(RotationAngle.z))); 
		float scaler = Size.x;

		// animation meshes will always have a parent
		// which is the root bone of the skeleton
		if ((*go)->IsMesh)
		{
			

			VQS_ModelToWorld = 
				VQS(VQS_TranslatorForAnimMesh, VQS_RotatorForAnimMesh, VQS_ScalerForAnimMesh) /** VQS_ModelToWorldForAnimMesh*/;

			VQS VQS_ParentModelToWorld =
				parent->GetGLObject()->Transform->VQS_ModelToWorld;

			if (initialState <= 1)
			{
				initialTransform = VQS_ParentModelToWorld.Translator.Vector.y;
				initialState++;
			}
			VQS_ParentModelToWorld.Translator.Vector.y = VQS_ParentModelToWorld.Translator.Vector.y - initialTransform;

			VQS_ModelToWorld =
				VQS_ParentModelToWorld * VQS_ModelToWorld;

			// animation mesh data is updated
			Position = VQS_ModelToWorld.Translator.Vector;
			RotationAngle = VQS_ModelToWorld.Rotator.GetEulerAngles();
			Size = VQS_ModelToWorld.GetUniformScale();

			/*int programID;
			glGetIntegerv(GL_CURRENT_PROGRAM, &programID);
			GLint bonesWeightLoc = glGetUniformLocation(programID, "VertexBonesWeights");
			glUniformMatrix4fv(bonesWeightLoc, DeformMatrices.size(), GL_FALSE, &DeformMatrices[0][0][0]);*/

			isMesh = 1;
		}
		else
		{
			if (!(*go)->IsBone)
			{
				//ModelToWorld = dummyModelToWorld;//translate * rotate *  scale; //  
				VQS_ModelToWorld = VQS(translator, rotator, scaler);
				int i = 0;
			}

			else
			{
				


				if ((*go)->GetName() == "Spine1")
					std::cout << "";
				if ((*go)->GetName() == "Spine2")
					std::cout << "";
				if ((*go)->GetName() == "LeftShoulder")
					std::cout << "";
				if ((*go)->GetName() == "LeftArm")
					std::cout << "";
				if ((*go)->GetName() == "LeftForeArm")
					std::cout << "";
				if ((*go)->GetName() == "LeftHand")
					std::cout << "";






				//ModelToWorld = ModelToWorldForBone *
				//TranslateForBoneMatrix * RotationForBoneMatrix * ScaleForBoneMatrix;


				Quaternion q1 = VQS_RotatorForBone;
				Quaternion q2 = VQS_RotatorForBoneIK;
				Quaternion tmp = q2*q1;

				VQS_ModelToWorld = 
					//VQS(glm::vec3(0.0f), q2, 1.0f) *
					VQS(VQS_TranslatorForBone, tmp, VQS_ScalerForBone) *
					VQS_ModelToWorldForBone;
				//ModelToWorldForBone = VQS_ModelToWorld.ToMatrix(false);
				
				/*Position = VQS_ModelToWorld.Translator.Vector;
				RotationAngle = VQS_ModelToWorld.Rotator.GetEulerAngles();
				Size = VQS_ModelToWorld.GetUniformScale();*/

				// uncomment this one for complete skeleton
				/*if ((*go)->GetGLObject()->Bonetype == BONE_TYPE::ROOT)
				{
					glm::vec3 trans = glm::vec3(0.0f, 0.0f, 0.0f);
					Quaternion rot = Quaternion::QuatFromAngles(glm::vec3(
						0.0f,
						glm::radians(12.0f),
						0.0f));
					float sca = 1.0f;
					VQS_ModelToWorld = VQS_ModelToWorld * VQS(trans, rot, sca);
				}*/
				

				if (parent)
				{
					// concatenation with matrices //
					//glm::mat4 ParentModelToWorld = 
					//	parent->GetGLObject()->Transform->ModelToWorld;
					//ModelToWorld =
					//	ParentModelToWorld * ModelToWorld;

					//// data for lines
					//glm::vec3 skew;
					//glm::vec4 perspective;
					//glm::quat rotation;
					//glm::decompose(ParentModelToWorld,
					//	ParentScale, rotation, ParentPosition, skew, perspective);
					//glm::vec3 eulerAngles = glm::eulerAngles(rotation);
					//ParentRotation = eulerAngles;
					//glm::decompose(ModelToWorld, Size, rotation,
					//	Position, skew, perspective);
					//eulerAngles = glm::eulerAngles(rotation);
					//RotationAngle = eulerAngles;

					//----//


					// concatenation with vqs
					VQS VQS_ParentModelToWorld =
						parent->GetGLObject()->Transform->VQS_ModelToWorld;
					VQS_ModelToWorld =
						VQS_ParentModelToWorld * VQS_ModelToWorld;

					// parent data is updated
					ParentPosition = VQS_ParentModelToWorld.Translator.Vector;
					ParentRotation = VQS_ParentModelToWorld.Rotator.GetEulerAngles();
					ParentScale = VQS_ParentModelToWorld.GetUniformScale();

					// bone data is updated
					Position = VQS_ModelToWorld.Translator.Vector;
					RotationAngle = VQS_ModelToWorld.Rotator.GetEulerAngles();
					Size = VQS_ModelToWorld.GetUniformScale();

				}

				// TODO
				// last step for skinning - store a reference in the root bone (hips)
				// and spread it out to the children bones.
				/*if (type BONE_TYPE::ROOT)
				{
					GameObject* mesh = (*go)->GetChild("Boy01_Body_Geo");

					std::string nameToQuery = (*go)->GetName();
					auto ptr = mesh->GetGLObject()->Transform->DeformMatrices.find(nameToQuery);
					ptr->second = *ptr->second
				}*/
			}
		}
		


		// after computing the final VQS system, it is converted to a matrix
		// because graphics cards does not support VQS.
		/*if((*go)->GetName() == "Hips")
			ModelToWorld = VQS_ModelToWorld.ToMatrix(false);
		else
			ModelToWorld = VQS_ModelToWorld.ToMatrix(true);*/

		ModelToWorld = VQS_ModelToWorld.ToMatrix(true);


		// camera is also updated with the updated matrix
		Camera->ModelToWorld = ModelToWorld;
		Camera3D* currentCamera = Camera;

		ModelTransform =
			currentCamera->Projection *
			currentCamera->View * 
			currentCamera->ModelToWorld;

		
	
		// finally, the transformation matrices are sent to the graphics card
		int programID;
		glGetIntegerv(GL_CURRENT_PROGRAM, &programID);

		
		GLint isLineLoc = glGetUniformLocation(programID, "IsLine");
		glUniform1i(isLineLoc, isLine);

		GLint isMeshLoc = glGetUniformLocation(programID, "IsAnimMesh");
		glUniform1i(isMeshLoc, isMesh);

		GLint projViewModel = glGetUniformLocation(programID, "ProjViewModel");
		glUniformMatrix4fv(projViewModel, 1, GL_FALSE, &ModelTransform[0][0]);

		GLint modelToWorldLoc = glGetUniformLocation(programID, "ModelMatrix");
		glUniformMatrix4fv(modelToWorldLoc, 1, GL_FALSE, &currentCamera->ModelToWorld[0][0]);

	}


	// this part is a previous implementation I made for 2D objects in orthographic scene.
	// it's not part of the animation assigment
	if ((*go)->GetGLObject()->Type == R2D)
	{
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), Position);
		glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(RotationAngle.z), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), Size);

		if (TransType == LOCAL)
			ModelToWorld = rotate * translate * scale;
		if (TransType == WORLD)
			ModelToWorld = translate * rotate * scale;

		ModelTransform = Camera2D->Projection * Camera2D->View * ModelToWorld;


		//int programID = shader->GetProgramID();

		GLint currentProgramId;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgramId);

		GLint projViewModel = glGetUniformLocation(currentProgramId, "ProjViewModel");
		glUniformMatrix4fv(projViewModel, 1, GL_FALSE, &ModelTransform[0][0]);
	}

	
	
}

void Transform::SetTransformMode(GLObject* globject)
{
	//renderType = globject->Type;
	
}
