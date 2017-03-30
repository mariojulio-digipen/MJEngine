#include "GLObject.h"
#include "Transform.h"


#include "RenderManager.h"
#include "Camera3D.h"
#include "Render3D.h"

#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <gtx\euler_angles.hpp>
#include <glm.hpp>

extern RenderManager* renderManager;

void GLObject::ResizeVaosNumber(int sections)
{
	vao.resize(sections, 0);
}

void GLObject::CreateBuffersFromFBX()
{
	this->Sections = 1;
	CreateBuffers();
}

void GLObject::CreateBuffers()
{
	if (this->DrawAnimMesh)
		VBOsNumber = 6;
	if (this->DrawBone)
		VBOsNumber = 4;

	int sections = vao.size();

	glGenVertexArrays(sections, &vao[0]);

	vbosPerSection.resize(sections);
	for (size_t i = 0; i < vbosPerSection.size(); i++)
	{
		vbosPerSection[i].resize(VBOsNumber, 0);
		glGenBuffers(VBOsNumber, &vbosPerSection[i][0]);
	}
	
	glGenVertexArrays(1, &lineVAO);
	glGenBuffers(1, &lineVBO);

	glGenVertexArrays(1, &dotVAO);
	glGenBuffers(1, &dotVBO);

	if (Name == "Cloth") // c'mon!! ¬¬' ....
	{
		glGenVertexArrays(1, &clothVAO);
		glGenBuffers(1, &clothVBO_V);
		glGenBuffers(1, &clothVBO_T);
		glGenBuffers(1, &clothVBO_N);
	}

	
}

void GLObject::deleteBuffers()
{
	int sections = vao.size();

	glDeleteVertexArrays(sections, &vao[0]);
	vao.clear();
	
	for (size_t i = 0; i < vbosPerSection.size(); i++)
	{
		glDeleteBuffers(VBOsNumber, &vbosPerSection[i][0]);
		vbosPerSection[i].clear();
	}
		
	vbosPerSection.clear();
	
	glDeleteVertexArrays(1, &lineVAO);
	glDeleteBuffers(1, &lineVBO);
	//
	glDeleteVertexArrays(1, &dotVAO);
	glDeleteBuffers(1, &dotVBO);

	glDeleteVertexArrays(1, &clothVAO);
	glDeleteBuffers(1, &clothVBO_V);
	glDeleteBuffers(1, &clothVBO_T);
	glDeleteBuffers(1, &clothVBO_N);

}

void GLObject::UseVAO(GLuint vaoId)
{
	glBindVertexArray(vao[vaoId]);
}



void GLObject::BuildVBOsFromFBX(int vboIndex, VBO_TYPE vboType)
{
	// TODO: optimize the VBOs from FBX...
}

VertexData GLObject::GetAnimationMeshData(VBO_TYPE dataType)
{
	VertexData returnData;
	// vector iterator is ok since often, there are no more than 2 animation meshes
	std::vector<Mesh> animMeshes = FModel->GetAnimationObject()->AnimationMeshes;
	std::vector<Mesh>::iterator animMeshIt = animMeshes.begin();
	while (animMeshIt != animMeshes.end())
	{
		Mesh* pMesh = &(*animMeshIt);
		std::string mName = pMesh->GetName();
		if (mName == this->Name)
		{
			switch (dataType)
			{
			case VERTEX:
				returnData.dataArray = pMesh->GetVertexData().dataArray;
				//int debugNumberData1 = pMesh->GetVertexData().dataArray.size() / 3;
				returnData.numberOfData = pMesh->GetVertexData().numberOfData;
				returnData.type = VBO_TYPE::VERTEX;
				break;
			case UV:
				returnData.dataArray = pMesh->GetUVData().dataArray;
				//int debugNumberData2 = pMesh->GetUVData().dataArray.size() / 2;
				returnData.numberOfData = pMesh->GetUVData().numberOfData;
				returnData.type = VBO_TYPE::UV;
				break;
			case NORMAL:
				returnData.dataArray = pMesh->GetVertexData().dataArray;
				//int debugNumberData3 = pMesh->GetVertexData().dataArray.size() / 3;
				returnData.numberOfData = pMesh->GetVertexData().numberOfData;
				returnData.type = VBO_TYPE::NORMAL;
				break;
			case TANGENT:
				break;
			case BONEID:
				returnData.dataIArray = pMesh->GetBonesIDs().dataArrayI;
				returnData.numberOfData = pMesh->GetBonesIDs().numberOfData;
				returnData.type = VBO_TYPE::BONEID;
				break;
			case BONEWEIGHT:
				returnData.dataArray = pMesh->GetBonesWeight().dataArray;
				returnData.numberOfData = pMesh->GetBonesWeight().numberOfData;
				returnData.type = VBO_TYPE::BONEWEIGHT;
				break;
			case MAX_TYPES:
				break;
			default:
				break;
			}
		}
		animMeshIt++;
	}


	return returnData;
}

VertexData GLObject::GetBoneMeshData(VBO_TYPE dataType)
{
	VertexData returnData;
	if (FModel == NULL)
		return returnData;

	std::unordered_map<std::string, Bone>::iterator
		meshDataIt = FModel->GetAnimationObject()->BonesMapByName.find(this->Name.c_str());
	
	if (meshDataIt != FModel->GetAnimationObject()->BonesMapByName.end())
	{
		Bone* pBone = &meshDataIt->second;
		switch (dataType)
		{
			case VBO_TYPE::VERTEX:
			{
				returnData.dataArray = pBone->MeshData.coordData;
				returnData.numberOfData = pBone->MeshData.coordData.size() / 3;
				returnData.type = VBO_TYPE::VERTEX;
				break;
			}
			case VBO_TYPE::UV:
			{
				returnData.dataArray = pBone->MeshData.textureData;
				returnData.numberOfData = pBone->MeshData.textureData.size() / 2;
				returnData.type = VBO_TYPE::UV;
				break;
			}
			case VBO_TYPE::NORMAL:
			{
				returnData.dataArray = pBone->MeshData.normalData;
				returnData.numberOfData = pBone->MeshData.normalData.size() / 3;
				returnData.type = VBO_TYPE::NORMAL;
				break;
			}
			case VBO_TYPE::TANGENT:
			{
				returnData.dataArray = pBone->MeshData.tangentData;
				returnData.numberOfData = pBone->MeshData.tangentData.size() / 3;
				returnData.type = VBO_TYPE::TANGENT;
				break;
			}
			default:
				break;
		}
		
	}
	return returnData;
}

VertexData GLObject::GetFBXMeshData(VBO_TYPE dataType)
{


	// TODO
	// I think this whole routine is too slow
	// gotta need to optimize it later
	VertexData returnData;
	//std::unordered_map<const char*, std::vector<VertexData>> meshData = FModel->NodesMeshData;
	std::unordered_map<std::string, std::vector<VertexData>>::iterator meshDataIt
		= FModel->NodesMeshData.find(this->Name.c_str());
	if (meshDataIt != FModel->NodesMeshData.end())
	{
		std::vector<VertexData> tmpV = meshDataIt->second;
		for (VertexData data : tmpV)
		{
			if (data.type == dataType)
				returnData = data;
		}
	}
	else
		std::cout << "No MeshData returned!! Careful!!!" << std::endl;

	return returnData;
}


void GLObject::BuildVBOsFromOBJ(int vboIndex, VBO_TYPE vboType, GLObject* cloneGLObj)
{
	std::string name = Name;


	if (Name == "Cloth")
		std::cout << "";


	int sectionCounter = 0;
	std::vector<int> vertexPerSection = this->VertexPerSection;
	std::vector<GLuint>::iterator vaoIt = vao.begin();
	while (vaoIt != vao.end())
	{
		glBindVertexArray((*vaoIt));

		if (DrawAnimMesh)
		{
			if (vboType == BONEID)
			{
				glBindBuffer(GL_ARRAY_BUFFER, vbosPerSection[sectionCounter][vboIndex]);
				if (cloneGLObj == NULL) // is an initial object?
				{
					BoneIDsData = GetAnimationMeshData(vboType);

					int sizeInBytes = (vertexPerSection[sectionCounter] * 4) * sizeof(int);
					int start = 0;
					if (sectionCounter > 0)
						for (int i = 0; i < sectionCounter; i++)
							start = start + vertexPerSection[i] * 4;

					glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &BoneIDsData.dataIArray[start], GL_STATIC_DRAW);
					glEnableVertexAttribArray(vboIndex);
					glVertexAttribIPointer(vboIndex, 4, GL_INT, 0, 0);
					Shader->BoneIdAttLocation = vboIndex;

					sectionCounter++;
					vaoIt = vaoIt + 1;
					continue;
				}
			}

			if (vboType == BONEWEIGHT)
			{
				glBindBuffer(GL_ARRAY_BUFFER, vbosPerSection[sectionCounter][vboIndex]);
				if (cloneGLObj == NULL) // is an initial object?
				{
					BoneWeightData = GetAnimationMeshData(vboType);

					int sizeInBytes = (vertexPerSection[sectionCounter] * 4) * sizeof(float);
					int start = 0;
					if (sectionCounter > 0)
						for (int i = 0; i < sectionCounter; i++)
							start = start + vertexPerSection[i] * 4;

					glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &BoneWeightData.dataArray[start], GL_STATIC_DRAW);
					glEnableVertexAttribArray(vboIndex);
					glVertexAttribPointer(vboIndex, 4, GL_INT, GL_FALSE, 0, 0);
					Shader->BoneWeightAttLocation = vboIndex;

					sectionCounter++;
					vaoIt = vaoIt + 1;
					continue;
				}
			}
		}

		if (vboType == VERTEX)
		{
			if (Name == "FMeshPart")
				printf("");
			glBindBuffer(GL_ARRAY_BUFFER, vbosPerSection[sectionCounter][vboIndex]);

			if (cloneGLObj == NULL) // is an initial object?
			{
				if(Model != NULL) // is obj?
					VertxData = *Model->GetVertexData();
				else 
				{
					if (DrawAnimMesh)
					{
						VertxData = GetAnimationMeshData(vboType);
					}
					else
					{
						if (!DrawBone) //  is a simple fbx mesh?
						{
							VertxData = GetFBXMeshData(vboType);
						}
						else
						{
							VertxData = GetBoneMeshData(vboType);
						}
					}
					
					
				}
					
			}	
			else
				VertxData = cloneGLObj->VertxData;
				

			int sizeInBytes = (vertexPerSection[sectionCounter]*3) * sizeof(float);
			int start = 0;
			if (sectionCounter > 0)
				for (int i = 0; i < sectionCounter; i++)
					start = start + vertexPerSection[i] * 3;
				

			glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &VertxData.dataArray[start], GL_STATIC_DRAW);
			glEnableVertexAttribArray(vboIndex);
			glVertexAttribPointer(vboIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);
			Shader->VertexAttLocation = vboIndex;

			sectionCounter++;
			vaoIt = vaoIt + 1;
			continue;
		}

		if (vboType == UV)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbosPerSection[sectionCounter][vboIndex]);

			if (cloneGLObj == NULL) // is an initial object?
			{
				if (Model != NULL) // is obj?
					TextureData = *Model->GetTexturesData();
				else
				{
					if (DrawAnimMesh)
					{
						TextureData = GetAnimationMeshData(vboType);
					}
					else
					{
						if (!DrawBone) //  is a simple fbx mesh?
						{
							TextureData = GetFBXMeshData(vboType);
						}
						else
						{
							TextureData = GetBoneMeshData(vboType);
						}
					}

					
				}
					
			}
			else
				TextureData = cloneGLObj->TextureData;



			int sizeInBytes = (vertexPerSection[sectionCounter]*2) * sizeof(float);
			int start = 0;
			if (sectionCounter > 0)
				for (int i = 0; i < sectionCounter; i++)
					start = start + vertexPerSection[i] * 2;

			glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &TextureData.dataArray[start], GL_STATIC_DRAW);
			glEnableVertexAttribArray(vboIndex);
			glVertexAttribPointer(vboIndex, 2, GL_FLOAT, GL_FALSE, 0, 0);
			Shader->TextureAttLocation = vboIndex;

			sectionCounter++;
			vaoIt = vaoIt + 1;
			continue;
		}

		if (vboType == NORMAL)
		{

			if (Name == "Hips")
			{
				std::cout << "";
			}

			VertexData tmp;

			if (DrawAnimMesh)
			{
				tmp = GetAnimationMeshData(vboType);
				if (tmp.numberOfData == 0)
				{
					std::cout << "No Normal data for object " << Name << std::endl;
					sectionCounter++;
					vaoIt = vaoIt + 1;
					continue;
				}
			}
			else
			{
				if (DrawBone && !BoneHandMade)
				{
					tmp = GetBoneMeshData(vboType);
					if (tmp.numberOfData == 0)
					{
						std::cout << "No Normal data for object " << Name << std::endl;
						sectionCounter++;
						vaoIt = vaoIt + 1;
						continue;
					}
				}
			}

			
			
			
			
			glBindBuffer(GL_ARRAY_BUFFER, vbosPerSection[sectionCounter][vboIndex]);


			if (cloneGLObj == NULL) // is an initial object?
			{
				if (Model != NULL) // is obj?
					NormalData = *Model->GetNormalsData();
				else
				{
					if (!DrawAnimMesh)
					{
						if (!DrawBone) //  is a simple fbx mesh?
						{
							NormalData = GetFBXMeshData(vboType);
						}
						else
						{
							NormalData = tmp;
						}
					}
					else
					{
						NormalData = tmp;
					}
					
				}
					
			}
			else
				NormalData = cloneGLObj->NormalData;
			

			int sizeInBytes = (vertexPerSection[sectionCounter]*3) * sizeof(float);
			int start = 0;
			if (sectionCounter > 0)
				for (int i = 0; i < sectionCounter; i++)
					start = start + vertexPerSection[i] * 3;

			glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &NormalData.dataArray[start], GL_STATIC_DRAW);
			glEnableVertexAttribArray(vboIndex);
			glVertexAttribPointer(vboIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);
			Shader->NormalAttLocation = vboIndex;

			sectionCounter++;
			vaoIt = vaoIt + 1;
			continue;
		}

		if (vboType == TANGENT)
		{
			VertexData tmp;
			if (Type == R3D)
			{
				tmp = GetBoneMeshData(vboType);
				if (tmp.numberOfData == 0)
				{
					//std::cout << "No Tangent data for object " << Name << std::endl;
					sectionCounter++;
					vaoIt = vaoIt + 1;
					continue;
				}
			}
			glBindBuffer(GL_ARRAY_BUFFER, vbosPerSection[sectionCounter][vboIndex]);

			if (cloneGLObj == NULL) // is an initial object?
			{
				if (Model != NULL) // is obj?
					TangentData = *Model->GetTangentsData();
				else
				{
					if (!DrawBone) //  is a simple fbx mesh?
					{
						TangentData = GetFBXMeshData(vboType);
					}
					else
					{
						TangentData = tmp;
					}
				}
					
			}
			else
				TangentData = cloneGLObj->TangentData;


			int sizeInBytes = (vertexPerSection[sectionCounter] * 3) * sizeof(float);
			int start = 0;
			if (sectionCounter > 0)
				for (int i = 0; i < sectionCounter; i++)
					start = start + vertexPerSection[i] * 3;

			glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &TangentData.dataArray[start], GL_STATIC_DRAW);
			glEnableVertexAttribArray(vboIndex);
			glVertexAttribPointer(vboIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);
			Shader->TangetAttLocation = vboIndex;

			sectionCounter++;
			vaoIt = vaoIt + 1;
			continue;
		}



		


	}

	// hack, fix this...
	if (DrawBone || this->Name == "Plane")
		fillLineBuffers();

	if (this->Name == "Plane")
		fillDotBuffers();

}

void GLObject::fillLineBuffers()
{
	// store line connector info if bone
	std::vector<float> points(6);
	points[0] = -0.0f;
	points[1] = +0.0f;
	points[2] = +0.0f;
	points[3] = +1.0f;
	points[4] = +1.0f;
	points[5] = +1.0f;

	glBindVertexArray(lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * 3 * sizeof(float), &points[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(Shader->VertexAttLocation);
	glVertexAttribPointer(Shader->VertexAttLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void GLObject::fillDotBuffers()
{
	std::vector<float> points(3);
	points[0] = 0.0f;
	points[1] = 0.0f;
	points[2] = 0.0f;

	glBindVertexArray(dotVAO);
	glBindBuffer(GL_ARRAY_BUFFER, dotVBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &points[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(Shader->VertexAttLocation);
	glVertexAttribPointer(Shader->VertexAttLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

}

void GLObject::Update()
{

	for (int i = 0; i < Sections; i++)
	{
		UseVAO(i);


		if (ExtraTextures.size() > 0)
			Shader->HasExtraTextures(true);
		else
			Shader->HasExtraTextures(false);
		Shader->IsExtraActive(false);

		Name;
		if (Textures.size() == 1)
		{
			Shader->ChooseTextureUnit(0, "textureSampler");
			Textures[0]->Use();
		}
		else if (Textures.size() > 1)
		{
			// This "if" statement is an example of texture blending 
			// I need to figure out how to modularize this feature, 
			// but for now, let's make blending occure here.
			/*if (i == 0 && Name == "Snaky")
			{
				Shader->IsExtraActive(true);
				Shader->ChooseTextureUnit(1, "extraSampler1");
				UseTexture(ExtraTextures[0]->GetId());
			}
			Shader->ChooseTextureUnit(0, "textureSampler");
			UseTexture(TexturesIdsToDraw[i]);*/
		}
		else
		{
			if (!IsScenePoint)
			{
				Shader->SetDebugModeOn();
				glLineWidth(3);
				glDrawArrays(GL_LINES, 0, SizeOfDataBySection[i]);
				Shader->SetDebugModeOff();
				continue;
			}
			else
			{
				int isDot = 1;
				GLint isdotLoc = glGetUniformLocation(Shader->GetProgramID(), "IsDot");
				glUniform1i(isdotLoc, isDot);

				glPointSize(10.0f);
				glDrawArrays(GL_POINTS, 0, SizeOfDataBySection[i]); // if point, always 1 vertex
				
				isDot = 0;
				isdotLoc = glGetUniformLocation(Shader->GetProgramID(), "IsDot");
				glUniform1i(isdotLoc, isDot);
				
				continue;
			}
			
		}
		
		if (DebugMode)
		{
			glLineWidth(6.0f);
			int loc = glGetUniformLocation(Shader->GetProgramID(), "DebugMode");
			glUniform1i(loc, 1);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glDrawArrays(GL_TRIANGLES, 0, SizeOfDataBySection[i]);

			glLineWidth(1.0f);
			loc = glGetUniformLocation(Shader->GetProgramID(), "DebugMode");
			glUniform1i(loc, 0);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glDrawArrays(GL_TRIANGLES, 0, SizeOfDataBySection[i]);
		}
		else
		{
			if (IsSelected)
			{
				int loc = glGetUniformLocation(Shader->GetProgramID(), "Selected");
				glUniform1i(loc, 1);
			}
			else
			{
				int loc = glGetUniformLocation(Shader->GetProgramID(), "Selected");
				glUniform1i(loc, 0);
			}

			
			glDrawArrays(GL_TRIANGLES, 0, SizeOfDataBySection[i]);
			
		}
		
		if (DrawAnimMesh)
		{
			int a = 0; a++;
		}
		

		if (ExtraTextures.size() > 0)
		{
			Shader->ChooseTextureUnit(1, "extraSampler1");
			Shader->ReleaseTextureUnit();
		}

		glBindTexture(GL_TEXTURE_2D, 0);

	}
	
}

// modify destructor also when fbx is included
GLObject::~GLObject()
{
	if (Transform)
		delete Transform;
	if (Model)
		delete Model;
	
	for (size_t i = 0; i < Textures.size(); i++)
	{
		if(Textures[i] != NULL)
			delete Textures[i];
	}
	Textures.clear();

	for (size_t i = 0; i < ExtraTextures.size(); i++)
	{
		if (ExtraTextures[i] != NULL)
			delete ExtraTextures[i];
	}
	ExtraTextures.clear();

	deleteBuffers();
}


// Drawing lines relies on scaling and VQS system only supports uniform scaling.
// Therefore, it is not possible to use VQS for this process. 
// Matrices are used instead.
void GLObject::DrawLine(glm::vec3 globalPosition1, glm::vec3 globalPosition2, glm::vec3 color, float width, 
	glm::vec3 offset, bool pivotOnGameObject)
{
	glBindVertexArray(lineVAO);
		glEnable(GL_DEPTH_TEST);
		int programID;
		glGetIntegerv(GL_CURRENT_PROGRAM, &programID);
		
		int isLine = 1;
		GLint isLineLoc = glGetUniformLocation(programID, "IsLine");
		glUniform1i(isLineLoc, isLine);

		GLint colorLoc = glGetUniformLocation(programID, "LineColor");
		glUniform3fv(colorLoc, 1, &color[0]);

		Camera3D* currentCamera = renderManager->GetRender3D()->CurrentCamera;
		GLint proj = glGetUniformLocation(programID, "Projection");
		glUniformMatrix4fv(proj, 1, GL_FALSE, &currentCamera->Projection[0][0]);

		GLint view = glGetUniformLocation(programID, "View");
		glUniformMatrix4fv(view, 1, GL_FALSE, &currentCamera->View[0][0]);

		// 1. Find vector between child and parent
		// 2. NON-UNIFORM Scale to the parent's postion
		// 3. Translate to child position. 
		// 4. Apply a transformation matrix and... Ta da!! :D
		glm::vec3 Vn;
		Vn = globalPosition1 - globalPosition2;
		
		glLineWidth(width);
		glm::mat4 S;
		glm::mat4 T;
		if (pivotOnGameObject)
		{
			S = glm::scale(glm::mat4(1.0f), Vn);
			T = glm::translate(glm::mat4(1.0f), Transform->Position + offset);
		}	
		else
		{
			S = glm::scale(glm::mat4(1.0f), Vn/**2.0f*/);
			T = glm::translate(glm::mat4(1.0f), globalPosition1);
		}
		
		GLint lineWorldLoc = glGetUniformLocation(programID, "LineT");
		glUniformMatrix4fv(lineWorldLoc, 1, GL_FALSE, &T[0][0]);
		lineWorldLoc = glGetUniformLocation(programID, "LineS");
		glUniformMatrix4fv(lineWorldLoc, 1, GL_FALSE, &S[0][0]);

		
		glDrawArrays(GL_LINES, 0, 2);

		glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}


void GLObject::DrawDot(glm::vec3 globalPosition1)
{
	glBindVertexArray(dotVAO);
		glEnable(GL_DEPTH_TEST);
		int programID;
		glGetIntegerv(GL_CURRENT_PROGRAM, &programID);

		int isDot = 1;
		GLint isdotLoc = glGetUniformLocation(programID, "IsDot");
		glUniform1i(isdotLoc, isDot);

		Camera3D* currentCamera = renderManager->GetRender3D()->CurrentCamera;
		GLint proj = glGetUniformLocation(programID, "Projection");
		glUniformMatrix4fv(proj, 1, GL_FALSE, &currentCamera->Projection[0][0]);

		GLint view = glGetUniformLocation(programID, "View");
		glUniformMatrix4fv(view, 1, GL_FALSE, &currentCamera->View[0][0]);


		glm::mat4 T;
		T = glm::translate(glm::mat4(1.0f), globalPosition1);
		glm::mat4 dotTransform = T;
		GLint lineWorldLoc = glGetUniformLocation(programID, "ModelMatrix");
		glUniformMatrix4fv(lineWorldLoc, 1, GL_FALSE, &dotTransform[0][0]);

		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, 1);
		glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);

}


void GLObject::ResetLineBuffers()
{
	glDeleteVertexArrays(1, &lineVAO);
	glDeleteBuffers(1, &lineVBO);

	glGenVertexArrays(1, &lineVAO);
	glGenBuffers(1, &lineVBO);

	fillLineBuffers();
	//
	/*glDeleteVertexArrays(1, &dotVAO);
	glDeleteBuffers(1, &dotVBO);*/


}