#include "Mesh.h"
#include <algorithm>

Mesh::Mesh()
{
}


Mesh::~Mesh()
{
}

void getVertexData(FbxMesh* mesh, std::vector<glm::vec3>* vertexData, const size_t vertexCount);
void convertKeyFrameToVQS(VQS* vqs, const FbxVector4* translator, const FbxQuaternion* rotator, const FbxVector4* scaling);
void getGeometryMatrix(FbxNode* node, FbxAMatrix& matrix);
void convertMToGLM(glm::mat4* dstMatrix, FbxAMatrix srcAMatrix);

template <class ForwardIt, class T, class Compare=std::less<>>
ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value, Compare comp = {})
{
	first = std::lower_bound(first, last, value, comp);
	return first != last && !comp(value, *first) ? first : last;

}

void Mesh::Initialize()
{
	mesh = node->GetMesh();
	const size_t vertexCount = static_cast<const size_t>(mesh->GetControlPointsCount());
	if (vertexCount == 0)
		return;
	totalVertexCount = vertexCount;
	FbxString nn = node->GetName();

	// just for debug - delete later
	if (nn == "Boy01_Body_Geo")
	{
		int a = 0;
		a++;
	}
	// this test failed in my project, I do not have anything in cache
	// since it is not in real time yet (I suppose this is because
	// there is nothing loaded in graphics card, anyway, let's figure out later)
	//const int deformersCountCache = mesh->GetDeformerCount(FbxDeformer::eVertexCache);
	//FbxDeformer* defCache = mesh->GetDeformer(0, FbxDeformer::eVertexCache);
	//FbxVertexCacheDeformer* cacheDeformer = static_cast<FbxVertexCacheDeformer*>(defCache);
	//FbxBool isActive = cacheDeformer->Active.Get();

}


void Mesh::StoreData()
{
	meshName = node->GetName();
	
	// retrieve and stores all the data from the mesh's Vertices
	storeVertexData();

	// retrieve and stores  all the data from the mesh's UVs
	storeUVData();

	// retrieve and stores  all the data from the mesh's Normals
	storeNormalData();

	// retrieves and stores the locations of the mesh in every frame
	storeKeyFrameData();

	// let's see what this info is good for...
	storeClusterCount();

	// retrieves texture files and set up model diffuse texture
	storeDiffuseTexture();

	// stores the mesh's skin type
	storeMeshSkinType();

	// stores the cluster mode of the mesh's skin
	storeMeshClusterMode();

	// inverse the relationship between bones and vertices and store weights
	storeBoneWeightsData();

	// stores the initial transform data
	storeInitialTransform();
}

void Mesh::storeKeyFrameData()
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

	MeshKeyFrame keyframe;
	std::vector<MeshKeyFrame> KeyFrames_temp;
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

		int childCount = node->GetChildCount();

		// documentation says that for skinning we need global position
		// let's see what happens then...
		//matrix = node->EvaluateLocalTransform(currentTime);
		matrix = node->EvaluateGlobalTransform(currentTime);
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
		KeyFrames.push_back(keyframe);
		i++;
	}

	// normalize times
	std::vector<long double> debugVec;
	std::vector<MeshKeyFrame>::iterator meshIt = KeyFrames.begin();
	std::vector<MeshKeyFrame>::iterator meshMaxIt = KeyFrames.end() - 1;
	while (meshIt != KeyFrames.end())
	{
		meshIt->time = meshIt->time / meshMaxIt->time;
		debugVec.push_back(meshIt->time);
		meshIt++;
	}
	return;

}


void Mesh::storeInitialTransform()
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
	convertKeyFrameToVQS(&transform.VQS_ModelToWorld, &translation, &rotator, &scaling);
}

void Mesh::storeVertexData()
{
	
	// Collect Coordinates data - these two lines will actually give me triangles - yay! :)
	size_t numPolygons = mesh->GetPolygonCount();
	size_t numPolygonVertices = mesh->GetPolygonVertexCount();
	int* vertices = mesh->GetPolygonVertices();
	
	for (size_t idx = 0; idx < numPolygonVertices; idx++)
		verticesVector.push_back(*(vertices + idx));

	vertexData.type = MESH_DATATYPE::MVERTEX;


	// get all the control points of the node (warning: not actual vertices!)
	size_t numControlPoints = mesh->GetControlPointsCount(); // only the vertices
	
	for (size_t cp = 0; cp < numControlPoints; cp++)
	{
		FbxVector4 cpoint = mesh->GetControlPointAt(cp);
		controlPointsVector.push_back(cpoint);
	}


	for (size_t v = 0; v < numPolygonVertices; v++)
	{
		int controlPointId = verticesVector[v];

		vertexData.dataArray.push_back(
			static_cast<float>(controlPointsVector[controlPointId].mData[0]));
		vertexData.dataArray.push_back(
			static_cast<float>(controlPointsVector[controlPointId].mData[1]));
		vertexData.dataArray.push_back(
			static_cast<float>(controlPointsVector[controlPointId].mData[2]));
	}
	vertexData.numberOfData = vertexData.dataArray.size() / 3;


}


void Mesh::storeUVData()
{
	// Collect UV data (texture coordinates)
	size_t numPolygonVertices = mesh->GetPolygonVertexCount();

	int uvCoordsCount = mesh->GetElementUVCount();
	bool meshHasUV = uvCoordsCount > 0;
	const FbxGeometryElementUV* uvElement = NULL;
	if (meshHasUV)
		uvElement = mesh->GetElementUV(0);
	FbxVector2 uvCoord;
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
	textureData.type = MESH_DATATYPE::MUV;
	textureData.numberOfData = textureData.dataArray.size() / 2;
}


void Mesh::storeNormalData()
{
	// Collect normal data
	size_t numPolygonVertices = mesh->GetPolygonVertexCount();

	int normalsCount = mesh->GetElementNormalCount();
	bool meshHasNormals = normalsCount > 0;
	const FbxGeometryElementNormal* normalsElement = NULL;
	if (meshHasNormals)
		normalsElement = mesh->GetElementNormal(0);
	FbxVector4 normal;
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
	normalData.type = MESH_DATATYPE::MNORMAL;
	normalData.numberOfData = normalData.dataArray.size() / 3;
}

void Mesh::storeBoneWeightsData()
{

	inverseBonesVerticesRelationship();

	size_t numPolygonVertices = mesh->GetPolygonVertexCount();

	std::vector<std::pair<int, glm::vec3>> points;
	for (size_t i = 0; i < numPolygonVertices; i++)
	{
		int vertexId = verticesVector[i];
		glm::vec3 point = glm::vec3(controlPointsVector[vertexId].mData[0], 
			controlPointsVector[vertexId].mData[1], controlPointsVector[vertexId].mData[2]);
		
		points.push_back(std::make_pair(vertexId, point));
	}
	// now look up indices in vertices vector, using point's indices.
	//std::vector<float> bWeigthsData;
	//std::vector<int> bIdData;
	for (size_t i = 0; i < points.size(); i++)
	{
		int index = points[i].first;
		auto ptr = binary_find(vertices.begin(), vertices.end(), index, compare_vertex_id<Vertex>());

		if (ptr->bonesData.size() > 4)
		{
			std::sort(ptr->bonesData.begin(), ptr->bonesData.end(), [](const BonesData& a, const BonesData& b)
			{
				return a.weight > b.weight;
			});

			double totalWeight = 0.0000;
			for (size_t iw = 0; iw < 4; iw++)
			{
				totalWeight += ptr->bonesData[iw].weight;
				//ptr->bonesData[iw].weight = ptr->bonesData[iw].weight / 
			}
			ptr->bonesData[0].weight = ptr->bonesData[0].weight / totalWeight;
			ptr->bonesData[1].weight = ptr->bonesData[1].weight / totalWeight;
			ptr->bonesData[2].weight = ptr->bonesData[2].weight / totalWeight;
			ptr->bonesData[3].weight = ptr->bonesData[3].weight / totalWeight;
		}
		


		for (size_t j = 0; j < ptr->bonesData.size(); j++)
		{
			bonesWeightData.dataArray.push_back(static_cast<float>(ptr->bonesData[j].weight));
			bonesIdData.dataArrayI.push_back(ptr->bonesData[j].id);
			if (j >= 3)
				break;
		}
		if (ptr->bonesData.size() == 1 )
		{
			bonesWeightData.dataArray.push_back(0.0f);
			bonesWeightData.dataArray.push_back(0.0f);
			bonesWeightData.dataArray.push_back(0.0f);
			bonesIdData.dataArrayI.push_back(10000); // means weight 0
			bonesIdData.dataArrayI.push_back(10000);
			bonesIdData.dataArrayI.push_back(10000);
		}
		else if (ptr->bonesData.size() == 2)
		{
			bonesWeightData.dataArray.push_back(0.0f);
			bonesWeightData.dataArray.push_back(0.0f);
			bonesIdData.dataArrayI.push_back(10000); // means weight 0
			bonesIdData.dataArrayI.push_back(10000);
		}
		else if (ptr->bonesData.size() == 3)
		{
			bonesWeightData.dataArray.push_back(0.0f);
			bonesIdData.dataArrayI.push_back(10000); // means weight 0
		}
		
		/*if (bonesIdData.dataArrayI.size() % 4 > 0)
		{
			int a = 0;
			a++;
		}*/
	}
	bonesWeightData.numberOfData = bonesWeightData.dataArray.size() / 4;
	bonesWeightData.floatsPerData = 4;
	bonesWeightData.intsPerData = 0;

	bonesIdData.numberOfData = bonesIdData.dataArray.size() / 4;
	bonesIdData.intsPerData = 4;
	bonesIdData.floatsPerData = 0;
	meshClustersData;

	return;
}

//MeshClusterData* Mesh::GetClusterBoneData(std::string boneName)
//{
//	std::vector<MeshClusterData>::iterator clustersIt = meshClustersData.begin();
//	while (clustersIt != meshClustersData.end())
//	{
//		if (clustersIt->influencerBoneName == boneName)
//			return &(*clustersIt);
//		clustersIt++;
//	}
//	return NULL;
//}

void Mesh::inverseBonesVerticesRelationship()
{
	std::vector<MeshClusterData>::iterator clusCurrent = meshClustersData.begin();
	std::vector<MeshClusterData>::iterator clusEnd = meshClustersData.end();

	for (int idx = 0; idx < totalVertexCount; idx++) // will look for every single vertex
	{
		Vertex vertex;
		vertex.influencedVertexIndex = idx;
		//vertex.point = 
		vertex.totalWeight = 0.0000;
		while (clusCurrent != clusEnd) // in every cluster bone
		{
			// search for the vertex index in the iwv triplet of each cluster
			std::vector<ClusterTriplet>::iterator iwvBegin = clusCurrent->iwv.begin();
			std::vector<ClusterTriplet>::iterator iwvEnd = clusCurrent->iwv.end();
			auto ptr = binary_find(iwvBegin, iwvEnd, idx, compare_vertex_id<ClusterTriplet>());
			if (ptr != iwvEnd)
			{
				BonesData boneData;
				boneData.boneName = clusCurrent->influencerBoneName;
				boneData.id = clusCurrent->influencerBoneId;
				boneData.weight = ptr->weigth;
				vertex.point = ptr->influencedVertex;
				vertex.totalWeight += boneData.weight;
				vertex.bonesData.push_back(boneData);
			}
			
			clusCurrent++;
		}
		vertices.push_back(vertex);
		clusCurrent = meshClustersData.begin();
	}
	clusCurrent++;
}

void Mesh::storeClusterCount()
{
	// test if mesh has deformers and if they are active
	//const int deformersCount = mesh->GetDeformerCount();
	//FbxDeformer* def = mesh->GetDeformer(0);
	//FbxVertexCacheDeformer* cacheDeformer = static_cast<FbxVertexCacheDeformer*>(def);
	//FbxBool isActive = cacheDeformer->Active.Get();


	// count how many shapes (??)
	const size_t shapes = mesh->GetShapeCount();
	const size_t skins = mesh->GetDeformerCount(FbxDeformer::eSkin);

	// from the sdk documentation :
	/*
		For example, imagine a mesh representing a humanoid, and imagine a skeleton
		made of bones. Each bone is represented by a node in FBX. To bind the geometry
		to the nodes, we create a skin (FbxSkin). The skin has many clusters, each one
		corresponding to a bone. Each node influences some control points of the mesh.
		A node has a high influence on some of the points (high weight) and lower
		influence on some other points (low weight). Some points of the mesh are
		not affected at all by the bone, so they would not be part of the corresponding
		cluster.
	*/
	
	if (skins > 0) // if has skins
	{
		// count number of clusters in the mesh
		// mesh might have many skins, so we loop over them
		int clusterCount = 0;
		for (size_t i = 0; i < skins; ++i)
		{
			FbxDeformer* tmpDef = mesh->GetDeformer(i, FbxDeformer::eSkin);
			FbxSkin* skin = static_cast<FbxSkin*>(tmpDef);
			size_t skinClusterCount = skin->GetClusterCount();

			// test to see how to get the clusters
			for (size_t i = 0; i < skinClusterCount; i++)
			{
				MeshClusterData mClusterData;
				// get the cluster
				FbxCluster* cluster = skin->GetCluster(i);
				//mClusterData.pCluster = cluster;
				
				// get cluster's influencer bone
				FbxNode* link = cluster->GetLink();
				mClusterData.influencerBoneName = link->GetName();
				mClusterData.influencerBoneId = i;
				
				// get cluster link's transformation matrix at binding time
				FbxAMatrix matrix;
				FbxAMatrix meshMatrix;
				FbxAMatrix geoMatrix;
				FbxAMatrix globalBindPoseMatrix;
				cluster->GetTransformLinkMatrix(matrix);
				cluster->GetTransformMatrix(meshMatrix);
				getGeometryMatrix(link, geoMatrix);
				globalBindPoseMatrix = matrix.Inverse() * meshMatrix * geoMatrix;
				glm::mat4 glm_bindmatrix;
				convertMToGLM(&glm_bindmatrix, globalBindPoseMatrix);
				mClusterData.globalBindPoseMatrix = glm_bindmatrix;// ::inverse(glm_bindmatrix);
				// store a vqs version of the matrix
				VQS VQS_GlobalBindPose;
				const FbxVector4 translation = globalBindPoseMatrix.GetT();
				const FbxQuaternion rotator = globalBindPoseMatrix.GetQ();
				const FbxVector4 scaling = globalBindPoseMatrix.GetS();
				convertKeyFrameToVQS(&VQS_GlobalBindPose, &translation, &rotator, &scaling);
				mClusterData.VQS_GlobalBindPose = VQS_GlobalBindPose;

				size_t lVertexIndexCount = cluster->GetControlPointIndicesCount();

				std::vector<ClusterTriplet> ivw;

				// using separte loops just for debugging.
				// this needs to be done in only one for
				std::vector<int> indicesArray;
				std::vector<double> weigths;
				for (size_t i = 0; i < lVertexIndexCount; i++)
				{
					indicesArray.push_back(cluster->GetControlPointIndices()[i]);
					weigths.push_back(cluster->GetControlPointWeights()[i]);
				}
				

				std::vector<glm::vec3> influencedVertices;
				for (size_t i = 0; i < indicesArray.size(); i++)
				{
					FbxVector4 vertex = mesh->GetControlPointAt(indicesArray[i]);

					float x, y, z;
					x = static_cast<float>(vertex.mData[0]);
					y = static_cast<float>(vertex.mData[1]);
					z = static_cast<float>(vertex.mData[2]);

					influencedVertices.push_back(glm::vec3(x, y, z));
				}


				for (size_t i = 0; i < indicesArray.size(); i++)
				{
					ClusterTriplet iwv; // index-weigth-vertex triplet
					iwv.influencedVertexIndex = indicesArray[i];
					iwv.influencedVertex = influencedVertices[i];
					iwv.weigth = weigths[i];
					mClusterData.iwv.push_back(iwv);
				}


				meshClustersData.push_back(mClusterData);

			}
			clusterCount += skinClusterCount;
		}
		totalClusterCount = clusterCount;
	}
}

void Mesh::storeDiffuseTexture()
{
	// for each material, search for each texture name that material could have,
	// then retrieve file name to create texture.
	//FbxSurfaceMaterial*
	std::string textureFileName;
	std::string textureName;
	FbxProperty texProperty;
	MeshMaterial thisNodeMaterial;
	size_t numMaterials = static_cast<size_t>(node->GetSrcObjectCount<FbxSurfaceMaterial>());
	for (size_t i = 0; i < numMaterials; i++)
	{
		FbxSurfaceMaterial* material = node->GetSrcObject<FbxSurfaceMaterial>(i);
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
								textureFileName = textureFileName.substr(idx + 1);

								textureName = texture->GetName();
							}
							thisNodeMaterial.textureNames.push_back(textureName);
							Texture* texture = new Texture("ArtAssets\\" + textureFileName);
							thisNodeMaterial.textures.push_back(texture);

						}
					}
				}
			}
		}
	}

	meshMaterials.push_back(thisNodeMaterial);
}

void Mesh::storeMeshSkinType()
{
	FbxSkin* skinDeformer = (FbxSkin*)mesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin::EType skinningType = skinDeformer->GetSkinningType();
	switch (skinningType)	
	{
	case fbxsdk::FbxSkin::eRigid:
		meshSkinningType = MESH_SKINNINGTYPE::RIGID;
		break;
	case fbxsdk::FbxSkin::eLinear:
		meshSkinningType = MESH_SKINNINGTYPE::LINEAR;
		break;
	case fbxsdk::FbxSkin::eDualQuaternion:
		meshSkinningType = MESH_SKINNINGTYPE::DUALQUATERNION;
		break;
	case fbxsdk::FbxSkin::eBlend:
		meshSkinningType = MESH_SKINNINGTYPE::BLEND;
		break;
	default:
		meshSkinningType = MESH_SKINNINGTYPE::MAX_MESH_SKINTYPE;
		break;
	}
}

void Mesh::storeMeshClusterMode()
{
	FbxSkin* skinDeformer = (FbxSkin*)mesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxCluster* cluster = skinDeformer->GetCluster(0);
	FbxCluster::ELinkMode clusterMode = cluster->GetLinkMode();
	switch (clusterMode)
	{
	case fbxsdk::FbxCluster::eNormalize:
		meshClusterMode = MESH_CLUSTERMODE::NORMALIZED;
		break;
	case fbxsdk::FbxCluster::eAdditive:
		meshClusterMode = MESH_CLUSTERMODE::ADDITIVE;
		break;
	case fbxsdk::FbxCluster::eTotalOne:
		meshClusterMode = MESH_CLUSTERMODE::TOTALONE;
		break;
	default:
		meshClusterMode = MESH_CLUSTERMODE::MAX_CLUSTER_MODES;
		break;
	}
}


// utility functions:

// creates an engine's standard array, instead of a FbxVector4.
void getVertexData(FbxMesh* mesh, std::vector<glm::vec3>* vertexData, const size_t vertexCount)
{
	FbxVector4* tmpArray = new FbxVector4[vertexCount];
	memcpy(tmpArray, mesh->GetControlPoints(), vertexCount * sizeof(FbxVector4));
	for (size_t i = 0; i < vertexCount; i++)
	{
		glm::vec3 data = glm::vec3(tmpArray[i].mData[0],
			tmpArray[i].mData[1], tmpArray[i].mData[2]);
		vertexData->push_back(data);
	}
}

// The VQS system is generated from the translate, quaternion 
// rotation and scale of the local transform matrix of a node
void convertKeyFrameToVQS(VQS* vqs, const FbxVector4* translator, const FbxQuaternion* rotator, const FbxVector4* scaling)
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


void getGeometryMatrix(FbxNode* node, FbxAMatrix& matrixGeo)
{
	matrixGeo.SetIdentity();

	const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);
	matrixGeo.SetT(lT);
	matrixGeo.SetR(lR);
	matrixGeo.SetS(lS);
}


void convertMToGLM(glm::mat4* dstMatrix, FbxAMatrix srcAMatrix)
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