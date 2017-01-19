#pragma once

#include "VQS.h"
#include "Texture.h"

#include <fbxsdk.h>
#include <vector>
#include <string>
#include <glm.hpp>

#define MESH_SECTIONS 1
#define VERTEX_DEFAULTFLOATS 3
#define VERTEX_DEFAULTINTS 4
#define VERTEX_DEFAULTNUMBER 0

enum MESH_DATATYPE
{
	MVERTEX,
	MCOLOR,
	MUV,
	MNORMAL,
	MTANGENT,
	MBONEID, // I think ids will be set in real time, using game objects id. Not here.
	MBONEWEIGHT,

	MAX_MTYPES
};

enum MESH_CLUSTERMODE
{
	NORMALIZED,
	ADDITIVE,
	TOTALONE,

	MAX_CLUSTER_MODES
};

enum MESH_SKINNINGTYPE
{
	RIGID,
	LINEAR,
	DUALQUATERNION,
	BLEND,

	MAX_MESH_SKINTYPE
};

struct MeshKeyFrame
{
	int index;
	long double time;
	// think if matrices will be needed later...
	VQS VQS_Transform;
};

struct MeshVertexData
{
	std::vector<int> dataArrayI;
	std::vector<float> dataArray;
	int floatsPerData = VERTEX_DEFAULTFLOATS;
	int intsPerData = VERTEX_DEFAULTINTS;
	int numberOfData = VERTEX_DEFAULTNUMBER;
	MESH_DATATYPE type;
};


struct MeshMaterial
{
	std::vector<Texture*> textures;
	std::vector<std::string> textureNames;
};

struct ClusterTriplet
{
	int influencedVertexIndex;
	double weigth;
	glm::vec3 influencedVertex;
};

struct MeshClusterData // basically, a bone	
{
	int influencerBoneId;
	std::string influencerBoneName;
	std::vector<ClusterTriplet> iwv; // idx_vtx_wght
	glm::mat4 globalBindPoseMatrix;
	VQS VQS_GlobalBindPose;
};

struct MeshTransform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4 ModelToWorld;
	VQS VQS_ModelToWorld;


};

template<class T>
struct compare_vertex_id
{
	//bool operator() (int id, const ClusterTriplet& u)
	bool operator() (int id, const T& u)
	{
		return id < u.influencedVertexIndex;
	}

	//bool operator() (const ClusterTriplet& u, int id)
	bool operator() (const T& u, int id)
	{
		return id > u.influencedVertexIndex;
	}
};

struct BonesData
{
	int id;
	std::string boneName;
	double weight;
};

struct Vertex
{
	int influencedVertexIndex;
	glm::vec3 point;
	std::vector<BonesData> bonesData;
	double totalWeight;
};

class Mesh
{
private:

	FbxMesh* mesh;
	FbxNode* node;
	FbxScene* scene;
	FbxAnimLayer* animLayer;
	FbxAnimStack* animStack;

	std::string meshName;

	MESH_SKINNINGTYPE meshSkinningType;
	MESH_CLUSTERMODE meshClusterMode;

	int totalClusterCount;
	int totalVertexCount; // control points

	std::vector<Vertex> vertices;
	std::vector<MeshClusterData> meshClustersData;

	MeshTransform transform;

	MeshVertexData vertexData;
	MeshVertexData textureData;
	MeshVertexData normalData;
	MeshVertexData bonesWeightData;
	MeshVertexData bonesIdData;

	// this two vars are to get vertices points by index
	std::vector<FbxVector4> controlPointsVector;
	std::vector<int> verticesVector;

	int sections;

	//std::vector<VertexToBonesData> vertexToBoneData;

	std::vector<MeshMaterial> meshMaterials;

	void storeInitialTransform();
	void storeVertexData();
	void storeUVData();
	void storeNormalData();
	void storeBoneWeightsData();
	void storeKeyFrameData();
	void storeClusterCount();
	void storeDiffuseTexture();
	void storeMeshSkinType();
	void storeMeshClusterMode();
	void inverseBonesVerticesRelationship();

	void storeTextureSections() { sections = MESH_SECTIONS; }

public:
	Mesh();
	~Mesh();

	std::vector<MeshKeyFrame> KeyFrames;

	void Initialize();

	void StoreData();
	//MeshClusterData* GetClusterBoneData(std::string boneName);
	std::vector<MeshClusterData>* GetClusterBoneData() { return &meshClustersData; }
	void SetScene(FbxScene* pScene) { this->scene = pScene; }
	void SetAnimStack(FbxAnimStack* pStack) { this->animStack = pStack; }
	void SetAnimationLayer(FbxAnimLayer* layer) { this->animLayer = layer; }
	void SetNode(FbxNode* node) { this->node = node; }

	std::string GetName() { return meshName; }
	int GetClustersCount() { return totalClusterCount; }
	int GetMeshSections() { return sections; }
	std::vector<MeshMaterial> GetMeshMaterials() { return meshMaterials; }
	MeshVertexData GetVertexData() { return vertexData; }
	MeshVertexData GetUVData() { return textureData; }
	MeshVertexData GetNormalData() { return normalData; }
	MeshVertexData GetBonesWeight() { return bonesWeightData; }
	MeshVertexData GetBonesIDs() { return bonesIdData; }
	std::vector<Vertex> GetVertices() { return vertices; }
	MeshTransform GetInitialTransform() { return transform; }
	MESH_SKINNINGTYPE GetSkinningType() { return meshSkinningType; }
	MESH_CLUSTERMODE GetClusterMode() { return meshClusterMode; }

};