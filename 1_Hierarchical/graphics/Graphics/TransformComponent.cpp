#include "TransformComponent.h"
#include <algorithm>

#include "ResourceManager.h"

extern ResourceManager* resourceManager;

extern Shader* shader;

//exceutes before serializing component
bool TransformComponent::Init()
{
	owner = GetOwner();
	if (glObject->Type == R3D)
	{
		/*glObject->Transform->Position = glm::vec3(0.0f, 0.0f, 0.0f);
		glObject->Transform->Size = glm::vec3(1.0f, 1.0f, 1.0f);
		glObject->Transform->RotationAngle = glm::vec3(0.0f, 0.0f, 0.0f);*/

	}

	if (glObject->Type == R2D)
	{
		/*glObject->Transform->Position = glm::vec3(0.0f, 0.0f, 0.0f);
		glObject->Transform->Size = glm::vec3(1.0f, 1.0f, 0.0f);*/
	}

	


	return true;
}

void TransformComponent::Update()
{
	
	

	glObject->Transform->Update(&owner);

	/*if(GetOwner()->IsBone)
		this->SetScale(glm::vec3(0.099f, 0.099f, 0.099f));*/

}


void TransformComponent::SetQuatRotator(Quaternion q)
{
	glObject->Transform->VQS_RotatorForBone = q;
}


void TransformComponent::Serialize(std::istringstream* objcontentStream)
{
	std::string line;
	std::getline((*objcontentStream), line);

	std::istringstream lineStream(line.c_str());
	std::string word;

	// VQS properties
	glm::vec3 translator;
	Quaternion rotator;
	float scaler;

	while ((*objcontentStream) >> word)
	{
		if (word == "#")
		{
			std::getline((*objcontentStream), line);
			continue;
		}
		if (word == "p")
		{
			float x, y, z;
			std::getline((*objcontentStream), line);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y,&z);

			// for matrix calculations:
			glObject->Transform->Position.x = x;
			glObject->Transform->Position.y = y;
			glObject->Transform->Position.z = z;

			// for VQS calculations:
			translator = glm::vec3(x, y, z);

			/*sscanf_s(line.c_str(), "%f %f %f",
				&glObject->Transform->Position.x,
				&glObject->Transform->Position.y,
				&glObject->Transform->Position.z);*/

			continue;
		}
		if (word == "r")
		{
			float x, y, z;
			std::getline((*objcontentStream), line);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			
			// for matrix calculations:
			glObject->Transform->RotationAngle.x = x;
			glObject->Transform->RotationAngle.y = y;
			glObject->Transform->RotationAngle.z = z;

			// for VQS calculations:
			rotator = Quaternion::QuatFromAngles(glm::vec3(
				glm::radians(x),
				glm::radians(y),
				glm::radians(z)));
			
			/*sscanf_s(line.c_str(), "%f %f %f",
				&glObject->Transform->RotationAngle.x,
				&glObject->Transform->RotationAngle.y,
				&glObject->Transform->RotationAngle.z);*/

			continue;
		}
		if (word == "s")
		{
			float x, y, z;
			std::getline((*objcontentStream), line);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);

			// for matrix calculations:
			glObject->Transform->Size.x = x;
			glObject->Transform->Size.y = y;
			glObject->Transform->Size.z = z;

			// for VQS calculations: (uniform scale)
			scaler = x;
			
			/*sscanf_s(line.c_str(), "%f %f %f",
				&glObject->Transform->Size.x,
				&glObject->Transform->Size.y,
				&glObject->Transform->Size.z);*/

			continue;
		}
		if (word == "cs")
		{
			std::getline((*objcontentStream), line);
			line.erase(
				std::remove_if(line.begin(),
					line.end(), isspace),
				line.end()
			);
			if(line == "global")
				glObject->Transform->TransType = TRANSFORM_TYPE::WORLD;
			else if(line == "local")
				glObject->Transform->TransType = TRANSFORM_TYPE::LOCAL;
			else
				glObject->Transform->TransType = TRANSFORM_TYPE::WORLD;
		}

		if (word == "end_TransformComponent")
		{
			VQS vqs = VQS(translator, rotator, scaler);
			glObject->Transform->VQS_ModelToWorldForBone = vqs;

			glm::vec3 VQS_TranslatorForBone = glm::vec3(0.0f, 0.0f, 0.0f);
			Quaternion VQS_RotatorForBone = Quaternion(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
			float VQS_ScalerForBone = 1.0f;
			glObject->Transform->VQS_TranslatorForBone = VQS_TranslatorForBone;
			glObject->Transform->VQS_RotatorForBone = VQS_RotatorForBone;
			glObject->Transform->VQS_ScalerForBone = VQS_ScalerForBone;

			if (glObject->Type == R2D)
			{
				float x = glObject->Transform->Position.x -
					owner->MainCamera2D->WinWidth*0.5f + glObject->Transform->Size.x;
				float y = glObject->Transform->Position.y +
					owner->MainCamera2D->WinHeight*0.5f - glObject->Transform->Size.y;
				glObject->Transform->Position = glm::vec3(x, y, 0.0f);
			}
			break;
		}
	}
	
}

void TransformComponent::Destroy()
{

}


void TransformComponent::CompleteExtraSetup()
{
	/*if (owner->IsBone)
		if(owner->GetParent())
			owner->GetGLObject()->Transform->ModelToWorld = owner->GetGLObject()->Transform->ModelToWorldForBone;*/
}