#pragma once

#include "Texture.h"
#include "Font.h"
#include "ModelLoader.h"
#include "MJFBXImporter.h"
#include "VQS.h"
#include "Shader.h"
#include "AnimationData.h"
#include <string>
#include <vector>
#include <unordered_map>

enum RENDER_TYPE
{
	R3D,
	R2D,

	MAX
};

struct GLObjectAnimKeyFrames
{
	int index;
	long double time;
	//glm::mat4 Matrix;
	VQS vqs;
};


class Transform;
class GLObject
{
private:

	std::vector<std::vector<GLuint>> vbosPerSection;
	std::vector<GLuint> vao;
	//int VBOsNumber = 4;
	int VBOsNumber = 4; // 4 for vertex, uv, normal and tangent by default.
	// if this gl object is for a mesh, then create 2 more for boneIDs and Weigths (vec4 each)
	//int IBOsNumber = 4;

	GLuint lineVAO = 0;
	GLuint lineVBO;
	GLuint dotVAO = 0;
	GLuint dotVBO;


	

	void deleteBuffers();

	void fillLineBuffers();
	void fillDotBuffers();

	

public:

	GLuint clothVAO = 0;
	GLuint clothVBO_V = 0;
	GLuint clothVBO_T = 0;
	GLuint clothVBO_N = 0;


	std::string Name;
	//int Oid;

	bool ActiveColor;
	bool DebugMode;

	std::string ActiveTextureName;
	int ActiveTextureUnit;

	Transform* Transform;
	Shader* GbuffShader;
	Shader* Shader;
	RENDER_TYPE Type;
	std::vector<Texture*> Textures;
	std::vector<Texture*> ExtraTextures;
	std::vector<GLuint> TexturesIdsToDraw;
	std::vector<std::string> TexturesNames;
	std::unordered_map<std::string, int> TexturesMap;
	int Sections;
	std::vector<int> VertexPerSection;


	VertexData TextureData;
	VertexData NormalData;
	VertexData TangentData;
	VertexData VertxData;
	VertexData BoneIDsData;
	VertexData BoneWeightData;

	std::vector<GLObjectAnimKeyFrames> KeyFramesData;

	ModelLoader* Model;
	MJFBXImporter* FModel;
	bool DrawBone;
	bool DrawAnimMesh;
	bool BoneHandMade;
	BONE_TYPE Bonetype;
	MESH_CLUSTERMODE MeshClusterMode;
	MESH_SKINNINGTYPE MeshSkinningType;


	int SizeOfData = 0;
	std::vector<int> SizeOfDataBySection;

	// For UI manipulation:
	bool IsSelected;

	// For simple transformation point rendering
	bool IsScenePoint;

	void Update();
	void ResizeVaosNumber(int sections);
	void CreateBuffers();
	void UseVAO(GLuint vaoId);
	void UseTexture(GLuint id) { glBindTexture(GL_TEXTURE_2D, id); }
	void BuildVBOsFromOBJ(int vboIndex, VBO_TYPE vboType, GLObject* cloneGLObj = NULL);
	//void BuildVBOsFromOBJ(int vboIndex, VBO_TYPE vboType, int att);


	GLObject() {}
	GLObject(bool activeColor, bool debugMode) :
		ActiveColor(activeColor), DebugMode(debugMode), DrawBone(false)
	{}
	~GLObject();


	// functions for FBX loading
	void CreateBuffersFromFBX();
	void BuildVBOsFromFBX(int vboIndex, VBO_TYPE vboType);
	VertexData GetFBXMeshData(VBO_TYPE dataType);
	VertexData GetAnimationMeshData(VBO_TYPE dataType);
	VertexData GetBoneMeshData(VBO_TYPE dataType);

	// utilities to draw primitives
	void DrawLine(glm::vec3 globalPosition1, glm::vec3 globalPosition2, glm::vec3 color, float width,
		glm::vec3 offset = glm::vec3(0.0f,0.0f,0.0f), bool pivotOnGameObject = true);
	void DrawDot(glm::vec3 globalPosition1);
	void ResetLineBuffers();
};

