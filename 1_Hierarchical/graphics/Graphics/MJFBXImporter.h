#pragma once
#include <fbxsdk.h>
#include <list>
#include <map>

#include "ModelLoader.h"




enum IOSettings
{
	MODEL,
	MATERIAL,
	TEXTURE,
	LINK,
	SHAPE,
	GOBO,
	ANIMATION,
	GLOBAL_SETTINGS
};

// useful structs
struct ParentHood
{
	std::string nodeName;
	std::vector<std::string> childrenName;
};

struct Materials
{
	std::vector<Texture*> textures;
	std::vector<std::string> textureNames;
	std::string nodeName;
};

struct TransformData
{
	glm::vec3 translation;
	glm::vec3 rotation;
	glm::vec3 scale;
};

class AnimationData;
class MJFBXImporter
{
private:
	FbxManager* mySdkMngr;
	FbxImporter* importer;
	FbxScene* scene;

	AnimationData* animation;

	std::map<std::string, VertexData> nodesCoordinatesData;
	std::map<std::string, VertexData> nodesTextureData;
	std::map<std::string, VertexData> nodesNormalData;
	std::map<std::string, VertexData> nodesTangentData;

	//std::map<std::string, TransformData> nodesTransformData;

	std::vector<int> sectionVertex;

	void traverseNodes(FbxNode* node);
	

	std::vector<Materials> textureMaterials;


	// tangents calculation

	void computeTangents(VertexData coordinatesData, VertexData textureData, VertexData normalData, VertexData* tangentData);
	void setOneTriangleCoords(VBO_TYPE type, glm::vec3* v1, glm::vec3* v2,
		glm::vec3* v3, unsigned int startIndex, VertexData* coordinatesData, VertexData* textureData);

public:

	std::vector<ParentHood> RelationsList;
	std::unordered_map<std::string, std::vector<VertexData>> NodesMeshData;
	std::unordered_map<std::string, TransformData> NodesMeshTransformData;

	MJFBXImporter();
	~MJFBXImporter();

	void InitializeSDKManager();
	void CreateScene();
	void DestroySDKObjects();

	void ImportItem(IOSettings setting);
	void Create(std::string fileName);
	void ReadNodes();

	// utility mesh functions
	std::vector<int> GetVertexPerSection() { return sectionVertex; }
	std::vector<Texture*> GetMaterialsTextures(std::string nodename);



	AnimationData* GetAnimationObject() { return animation; }



};

