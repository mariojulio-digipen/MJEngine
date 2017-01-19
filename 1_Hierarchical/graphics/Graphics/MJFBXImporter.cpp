#include <iostream>
#include <exception>
#include <SDL.h>
#include <algorithm>
#include <sstream>
#include <fbxsdk.h>

#include "MJFBXImporter.h"
#include "UIManager.h"
#include "AnimationData.h"

extern UIManager* uiManager;

MJFBXImporter::MJFBXImporter()
{
}


MJFBXImporter::~MJFBXImporter()
{
}

void MJFBXImporter::InitializeSDKManager()
{
	// memory
	mySdkMngr = FbxManager::Create();
	if (!mySdkMngr)
		throw std::exception("Initilization returned NULL - Not a valid version of FBX SDK -");
	FBXSDK_printf("Autodesk FBX SDK version %s\n", mySdkMngr->GetVersion());

	// io settings
	FbxIOSettings* ioSettings = FbxIOSettings::Create(mySdkMngr, IOSROOT);
	mySdkMngr->SetIOSettings(ioSettings);
	mySdkMngr->GetIOSettings()->SetBoolProp(IMP_FBX_MODEL, false);
	mySdkMngr->GetIOSettings()->SetBoolProp(IMP_FBX_MATERIAL, false);
	mySdkMngr->GetIOSettings()->SetBoolProp(IMP_FBX_TEXTURE, false);
	mySdkMngr->GetIOSettings()->SetBoolProp(IMP_FBX_LINK, false);
	mySdkMngr->GetIOSettings()->SetBoolProp(IMP_FBX_SHAPE, false);
	mySdkMngr->GetIOSettings()->SetBoolProp(IMP_FBX_GOBO, false);
	mySdkMngr->GetIOSettings()->SetBoolProp(IMP_FBX_ANIMATION, false);
	mySdkMngr->GetIOSettings()->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, false);

	// initialie pointer to AnimationDataClass. 
	// Goal: To get animation data
	//AnimationData* animation = new AnimationData(mySdkMngr, scene);
	
}

void MJFBXImporter::DestroySDKObjects()
{
	if (mySdkMngr)
		mySdkMngr->Destroy();
}

void MJFBXImporter::ImportItem(IOSettings setting)
{
	const char* globalSetting = NULL;
	switch (setting)
	{
		case MODEL:
			globalSetting = IMP_FBX_MODEL;
			break;
		case MATERIAL:
			globalSetting = IMP_FBX_MATERIAL;
			break;
		case TEXTURE:
			globalSetting = IMP_FBX_TEXTURE;
			break;
		case LINK:
			globalSetting = IMP_FBX_LINK;
			break;
		case ANIMATION:
			globalSetting = IMP_FBX_ANIMATION;
			break;
		case SHAPE:
			globalSetting = IMP_FBX_SHAPE;
			break;
		case GOBO:
			globalSetting = IMP_FBX_GOBO;
			break;
		case GLOBAL_SETTINGS:
			globalSetting = IMP_FBX_GLOBAL_SETTINGS;
			break;
		default:
			throw std::exception(std::string("Global setting \"%s\" is undefined",
				globalSetting).c_str());
	}
	mySdkMngr->GetIOSettings()->SetBoolProp(globalSetting, true);
}


void MJFBXImporter::Create(std::string fileName)
{
	importer = FbxImporter::Create(mySdkMngr, "MJImporter");
	bool importOk = importer->Initialize(fileName.c_str(), -1, mySdkMngr->GetIOSettings());
	if (!importOk)
	{
		std::cout << "Importer initialization failed!" << std::endl;
		std::cout << std::string("Actual FBX error: %s",
			importer->GetStatus().GetErrorString()) << std::endl;
	}
	
}

void MJFBXImporter::CreateScene()
{
	scene = FbxScene::Create(mySdkMngr, "MJEngine's Scene");
	if (!scene)
		throw std::exception("Something went wrong! - Scene is NULL -");
	bool importOk = importer->Import(scene);
	if (!importOk)
		throw std::exception("Something went wrong while importing the scene!");
	importer->Destroy();
	animation = new AnimationData(mySdkMngr, scene);
}

void MJFBXImporter::ReadNodes()
{
	FbxNode* parentNode = scene->GetRootNode();
	if (!parentNode)
		throw std::exception("Something went wrong! - Root node is NULL");
	traverseNodes(parentNode);

	// once I have all the mesh info, reorder this in a single map
	std::map<std::string, VertexData>::iterator coordIt = nodesCoordinatesData.begin();
	std::map<std::string, VertexData>::iterator texIt = nodesTextureData.begin();
	std::map<std::string, VertexData>::iterator normIt = nodesNormalData.begin();
	std::map<std::string, VertexData>::iterator tangIt = nodesTangentData.begin();

	size_t nodesFound = 0;
	
	while (coordIt != nodesCoordinatesData.end())
	{
		std::string nodeName;
		std::vector<VertexData> nodeData(4);
		nodeName = coordIt->first;
		nodeData[0] = coordIt->second;
		nodeData[1] = texIt->second;
		nodeData[2] = normIt->second;
		nodeData[3] = tangIt->second;

		NodesMeshData.insert(std::make_pair(nodeName, nodeData));
		coordIt++; texIt++; normIt++; tangIt++;
	}
	nodesCoordinatesData.clear();
	nodesTextureData.clear();
	nodesNormalData.clear();
	nodesTangentData.clear();

}

void MJFBXImporter::traverseNodes(FbxNode* node)
{
	// debug
	const char* nodeName = node->GetName();

	size_t numChildren = static_cast<size_t>(node->GetChildCount());
	ParentHood relation;
	relation.nodeName = nodeName;
	for (size_t i = 0; i < numChildren; i++)
	{
		const char* n = node->GetChild(i)->GetName();
		relation.childrenName.push_back(n);
	}
	RelationsList.push_back(relation);

	//To get the number of children of the root node
	size_t parentChildrenNumber = static_cast<size_t>(node->GetChildCount());
	FbxNode* childNode = NULL;
	for (size_t i = 0; i < parentChildrenNumber; i++)
	{
		childNode = node->GetChild(i);

		const char* childNodeName = childNode->GetName();
		size_t childChildrenNumber = static_cast<size_t>(childNode->GetChildCount());

		// get mesh info
		FbxMesh* mesh = childNode->GetMesh();
		if (!mesh)
			throw std::exception("Something went wrong! - mesh was NULL");

		// get all the control points of the node (warning: not actual vertices!)
		size_t numControlPoints = mesh->GetControlPointsCount(); // only the vertices
		std::vector<FbxVector4> controlPointsVector;
		for (size_t cp = 0; cp < numControlPoints; cp++)
		{
			FbxVector4 cpoint = mesh->GetControlPointAt(cp);
			controlPointsVector.push_back(cpoint);
		}


		// Collect Transform Data
		TransformData childNodeTransform;
		FbxDouble3 translation = childNode->LclTranslation.Get();
		glm::vec3 newTrans = glm::vec3(static_cast<float>(translation.mData[0]),
			static_cast<float>(translation.mData[1]),
			static_cast<float>(translation.mData[2]));

		FbxDouble3 rotation = childNode->LclRotation.Get();
		glm::vec3 newRotation = glm::vec3(static_cast<float>(rotation.mData[0]),
			static_cast<float>(rotation.mData[1]),
			static_cast<float>(rotation.mData[2]));

		FbxDouble3 scale = childNode->LclScaling.Get();
		glm::vec3 newScale = glm::vec3(static_cast<float>(scale.mData[0]),
			static_cast<float>(scale.mData[1]),
			static_cast<float>(scale.mData[2]));

		childNodeTransform.translation = newTrans;
		childNodeTransform.rotation = newRotation;
		childNodeTransform.scale = newScale;
		
		NodesMeshTransformData.insert(std::make_pair(childNodeName, childNodeTransform));

		

		// Collect Coordinates data - these two lines will actually give me triangles - yay! :)
		size_t numPolygons = mesh->GetPolygonCount();
		size_t numPolygonVertices = mesh->GetPolygonVertexCount();
		int* vertices = mesh->GetPolygonVertices();
		std::vector<int> verticesVector;
		for (size_t idx = 0; idx < numPolygonVertices; idx++)
			verticesVector.push_back(*(vertices + idx));

		VertexData coordinatesData;
		coordinatesData.type = VBO_TYPE::VERTEX;
		for (size_t v = 0; v < numPolygonVertices; v++)
		{
			int controlPointId = verticesVector[v];

			coordinatesData.dataArray.push_back(
				static_cast<float>(controlPointsVector[controlPointId].mData[0]));
			coordinatesData.dataArray.push_back(
				static_cast<float>(controlPointsVector[controlPointId].mData[1]));
			coordinatesData.dataArray.push_back(
				static_cast<float>(controlPointsVector[controlPointId].mData[2]));
		}
		coordinatesData.numberOfData = coordinatesData.dataArray.size() / 3;
		nodesCoordinatesData.insert(
			std::make_pair(childNodeName, coordinatesData));
	
		// this line will be required in the future in order to 
		// count sections that a paticular texture belongs to
		//sectionVertex.push_back(coordinatesData.numberOfData); 
		//

		// Collect UV data (texture coordinates)
		int uvCoordsCount = mesh->GetElementUVCount();
		bool meshHasUV = uvCoordsCount > 0;
		const FbxGeometryElementUV* uvElement = NULL;
		if (meshHasUV)
			uvElement = mesh->GetElementUV(0);
		FbxVector2 uvCoord;
		VertexData textureData;
		textureData.type = VBO_TYPE::UV;
		textureData.floatsPerData = 2;
		for (size_t i = 0; i < numPolygonVertices; i++)
		{
			if (meshHasUV)
			{
				int uvIndex = i;
				if (uvElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					uvIndex = uvElement->GetIndexArray().GetAt(i);
				uvCoord = uvElement->GetDirectArray().GetAt(uvIndex);
				textureData.dataArray.push_back(static_cast<float>(uvCoord[0]));
				textureData.dataArray.push_back(static_cast<float>(uvCoord[1]));
			}
		}
		textureData.numberOfData = textureData.dataArray.size() / 2;
		nodesTextureData.insert(
			std::make_pair(childNodeName, textureData));

		// Collect normal data
		int normalsCount = mesh->GetElementNormalCount();
		bool meshHasNormals = normalsCount > 0;
		const FbxGeometryElementNormal* normalsElement = NULL;
		if (meshHasNormals)
			normalsElement = mesh->GetElementNormal(0);
		FbxVector4 normal;
		VertexData normalData;
		normalData.type = VBO_TYPE::NORMAL;
		for (size_t i = 0; i < numPolygonVertices; i++)
		{
			if (meshHasNormals)
			{
				int normalIndex = i;
				if (normalsElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					normalIndex = normalsElement->GetIndexArray().GetAt(i);
				normal = normalsElement->GetDirectArray().GetAt(normalIndex);
				normalData.dataArray.push_back(static_cast<float>(normal[0]));
				normalData.dataArray.push_back(static_cast<float>(normal[1]));
				normalData.dataArray.push_back(static_cast<float>(normal[2]));
			}
		}
		normalData.numberOfData = normalData.dataArray.size() / 3;
		nodesNormalData.insert(
			std::make_pair(childNodeName, normalData));

		// Collect tangent data
		/*int tangentsCount = mesh->GetElementTangentCount();
		bool meshHasTangents = tangentsCount > 0;
		const FbxGeometryElementTangent* tangentsElement = NULL;
		if (meshHasTangents)
			tangentsElement = mesh->GetElementTangent(0);
		FbxVector4 tangent;
		VertexData tangentData;
		tangentData.type = VBO_TYPE::TANGENT;
		for (size_t i = 0; i < numPolygonVertices; i++)
		{
			if (meshHasTangents)
			{
				int tangentIndex = i;
				if (tangentsElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
					tangentIndex = tangentsElement->GetIndexArray().GetAt(i);
				tangent = tangentsElement->GetDirectArray().GetAt(tangentIndex);
				tangentData.dataArray.push_back(static_cast<float>(tangent[0]));
				tangentData.dataArray.push_back(static_cast<float>(tangent[1]));
				tangentData.dataArray.push_back(static_cast<float>(tangent[2]));
			}
		}
		*/

		VertexData tangentData;
		tangentData.type = VBO_TYPE::TANGENT;
		tangentData.numberOfData = tangentData.dataArray.size() / 3;
		//VertexData coordinatesData, VertexData textureData, VertexData normalData, VertexData* tangentData

		computeTangents(coordinatesData, textureData, normalData, &tangentData);
		nodesTangentData.insert(
			std::make_pair(childNodeName, tangentData));


		
		// for each material, search for each texture name that material could have,
		// then retrieve file name to create texture.
		//FbxSurfaceMaterial*
		std::string textureFileName;
		std::string textureName;
		FbxProperty texProperty;
		Materials thisNodeMaterials;
		thisNodeMaterials.nodeName = childNodeName;
		size_t numMaterials = static_cast<size_t>(childNode->GetSrcObjectCount<FbxSurfaceMaterial>());
		for (size_t i = 0; i < numMaterials; i++)
		{
			FbxSurfaceMaterial* material = childNode->GetSrcObject<FbxSurfaceMaterial>(i);
			if (material)
			{
				FBXSDK_FOR_EACH_TEXTURE(i)
				{
					texProperty = material->FindProperty(FbxLayerElement::sTextureChannelNames[i]);
					if (texProperty.IsValid())
					{
						int texCount = texProperty.GetSrcObjectCount<FbxTexture>();
						for (int j = 0; j < texCount; j++)
						{

							FbxTexture* texture = texProperty.GetSrcObject<FbxTexture>(j);
							if (texture)
							{
								FbxFileTexture* textureFile = FbxCast<FbxFileTexture>(texture);

								if (textureFile)
								{
									textureFileName = textureFile->GetFileName();

									size_t idx = textureFileName.find_last_of("\\");
									textureFileName = textureFileName.substr(idx+1);

									textureName = texture->GetName();
								}
								thisNodeMaterials.textureNames.push_back(textureName);
								Texture* texture = new Texture("ArtAssets\\" + textureFileName);
								thisNodeMaterials.textures.push_back(texture);

								/*std::unordered_map<std::string, int>::const_iterator it = texturesMap.find(tmpWord);
								int mytexId;
								if (it != texturesMap.end())
									mytexId = it->second;
								else
									mytexId = id;
								texturesMap.insert(std::make_pair(tmpWord, ++id));*/

							}
							//Here we have to check if it's layeredtextures, or just textures:
							/*FbxLayeredTexture* layeredTexture = texProperty.GetSrcObject<FbxLayeredTexture>(j);
							if (layeredTexture)
							{
								int numTextures = layeredTexture->GetSrcObjectCount<FbxTexture>();
								for (int k = 0; k < numTextures; k++)
								{
									FbxTexture* texture = layeredTexture->GetSrcObject<FbxTexture>(k);
									if (texture)
									{
										FbxFileTexture* textureFile = FbxCast<FbxFileTexture>(texture);
										if (textureFile)
											textureFileName = textureFile->GetFileName();
									}

								}
							}
							else
							{
								FbxTexture* texture = texProperty.GetSrcObject<FbxTexture>(j);
								if (texture)
								{
									FbxFileTexture* textureFile = FbxCast<FbxFileTexture>(texture);
									
									if (textureFile)
									{
										textureFileName = textureFile->GetFileName();
										textureName = texture->GetName();
									}
									thisNodeMaterials.textureNames.push_back(textureName);
									//TextureMaterials.push_back();
									Texture* texture = new Texture("ArtAssets\\" + textureFileName);
									thisNodeMaterials.textures.push_back(texture);
								}

							}*/
						}
					}
				}
			}
		}

		textureMaterials.push_back(thisNodeMaterials);
		traverseNodes(childNode);

	}


}

std::vector<Texture*> MJFBXImporter::GetMaterialsTextures(std::string nodename)
{
	std::vector<Texture*> ret;
	std::vector<Materials>::iterator matsIt = textureMaterials.begin();
	while (matsIt != textureMaterials.end())
	{
		if ((*matsIt).nodeName == nodename)
		{
			ret = (*matsIt).textures;
			break;
		}
		matsIt++;
	}
	return ret;
}


void MJFBXImporter::computeTangents(VertexData coordinatesData, VertexData textureData, VertexData normalData, VertexData* tangentData)
{
	//std::vector<glm::vec3> tangentsSdir;
	//std::vector<glm::vec3> tangentsTdir;
	///*size_t tempUV = tempTextureData->dataArray.size() / 3;
	//size_t tempCoords = tempCoordinatesData->dataArray.size() / 3;*/

	//// for each triangle
	//size_t numTriangles = static_cast<size_t>(coordinatesData.numberOfData / 9);
	//for (size_t i = 0; i < numTriangles; i++)
	//{
	//	glm::vec3 v1, v2, v3, E1, E2;
	//	glm::vec3 vt1, vt2, vt3, M, W;
	//	glm::vec3 sDir, tDir;

	//	setOneTriangleCoords(VERTEX, &v1, &v2, &v3, i, &coordinatesData, &textureData);
	//	setOneTriangleCoords(UV, &vt1, &vt2, &vt3, i, &coordinatesData, &textureData);

	//	// E1//W; E2//M
	//	E1 = v2 - v1;
	//	E2 = v3 - v1;

	//	W = vt2 - vt1;
	//	M = vt3 - vt1;

	//	// 1 / Determinant:
	//	float A = 1 / (W.s*M.t - W.t*M.s);

	//	sDir = (M.t*E1 - M.s*E2)*A;
	//	tDir = (W.s*E2 - W.t*E1)*A;

	//	tangentsSdir.push_back(sDir);
	//	tangentsSdir.push_back(sDir);
	//	tangentsSdir.push_back(sDir);

	//	tangentsTdir.push_back(tDir);
	//	tangentsTdir.push_back(tDir);
	//	tangentsTdir.push_back(tDir);

	//	int a = 1;
	//}

	//// for each vertex
	//size_t tangSize = tangentsTdir.size();
	//size_t vertexCount = coordinatesData.numberOfData / 3;
	//for (size_t i = 0; i < vertexCount; i++)
	//{
	//	unsigned int startIndex = 3 * i;
	//	const glm::vec3 n = glm::vec3(
	//		normalData.dataArray[startIndex],
	//		normalData.dataArray[startIndex + 1],
	//		normalData.dataArray[startIndex + 2]
	//	);
	//	const glm::vec3 ts = tangentsSdir[i];

	//	const glm::vec3 tt = tangentsTdir[i];

	//	glm::vec3 tangent = (ts - n*glm::dot(n, ts));
	//	tangent = glm::normalize(tangent);

	//	tangentData->dataArray.push_back(tangent.x);
	//	tangentData->dataArray.push_back(tangent.y);
	//	tangentData->dataArray.push_back(tangent.z);
	//}
	//tangentData->numberOfData = tangentData->dataArray.size();
	//tangentsSdir.clear();
	//tangentsTdir.clear();
}

void MJFBXImporter::setOneTriangleCoords(VBO_TYPE type, glm::vec3* v1, glm::vec3* v2,
	glm::vec3* v3, unsigned int idx, VertexData* coordinatesData, VertexData* textureData)
{
	if (type == VERTEX)
	{
		unsigned int startIndex = 9 * idx;
		(*v1) = glm::vec3(
			coordinatesData->dataArray[startIndex],
			coordinatesData->dataArray[startIndex + 1],
			coordinatesData->dataArray[startIndex + 2]
		);
		(*v2) = glm::vec3(
			coordinatesData->dataArray[startIndex + 3],
			coordinatesData->dataArray[startIndex + 4],
			coordinatesData->dataArray[startIndex + 5]
		);
		(*v3) = glm::vec3(
			coordinatesData->dataArray[startIndex + 6],
			coordinatesData->dataArray[startIndex + 7],
			coordinatesData->dataArray[startIndex + 8]
		);
	}

	if (type == UV)
	{
		unsigned int startIndex = 6 * idx;
		(*v1) = glm::vec3(
			textureData->dataArray[startIndex],
			textureData->dataArray[startIndex + 1],
			0.0f
		);
		(*v2) = glm::vec3(
			textureData->dataArray[startIndex + 2],
			textureData->dataArray[startIndex + 3],
			0.0f
		);
		(*v3) = glm::vec3(
			textureData->dataArray[startIndex + 4],
			textureData->dataArray[startIndex + 5],
			0.0f
		);
	}


}