#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <glm.hpp>
#include "Texture.h"
#include "Font.h"

const float vertex[3] =
{
	0.0f, 0.0f, 0.0f
};

const float quad2D[8 * 3] =
{
	1.0,  1.0,  0.0, // Top right front 
	1.0, -1.0,  0.0, // Bottom right front 

	1.0, -1.0,  0.0, // Bottom right front 
	-1.0, -1.0,  0.0, // Bottom left front

	- 1.0, -1.0,  0.0, // Bottom left front
	- 1.0,  1.0,  0.0, // Top left front

	- 1.0,  1.0,  0.0, // Top left front
	1.0,  1.0,  0.0    // Top right front 

	//-1.0,  1.0,  0.0, // Top left front 

	//1.0, -1.0,  0.0, // Bottom right front
	
	
};


enum VBO_TYPE
{
	VERTEX,
	COLOR,
	UV,
	NORMAL,
	TANGENT,
	BONEID,
	BONEWEIGHT,

	MAX_TYPES
};
//typedef struct VertexData;

struct VertexData
{
	std::vector<int> dataIArray;
	std::vector<float> dataArray;
	int floatsPerData = 3;
	int numberOfData = 0;
	VBO_TYPE type;
};

class ModelLoader
{
private:
	int vertexCounter;
	int sections;
	std::vector<int> sectionVertex;
	VertexData* tempCoordinatesData;
	VertexData* tempTextureData;
	VertexData* tempNormalData;

	std::vector<int> indexBufferData;
	int indexBufferSize;
	
	std::vector<int> coordsIndexData;
	std::vector<int> textureIndexData;
	std::vector<int> normalIndexData;

	std::vector<std::string> materialLibs;
	std::vector<Texture*> textures;
	std::vector<std::string> textureNames;

	

	std::unordered_map<std::string, int> texturesMap;

	std::string fileContent;
	std::stringstream readFile(std::string fileName);
	void initializeModelPath(std::string fileName);
	void parseMaterial(std::string fileName);
	

	VertexData* coordinatesData;
	VertexData* textureData;
	VertexData* normalData;
	VertexData* tangentData;
	VertexData* colorsData;

	size_t triangleCountFromFile;
	std::vector<glm::vec3> tangentsSdir;
	std::vector<glm::vec3> tangentsTdir;
	void computeTangents();
	void setOneTriangleCoords(VBO_TYPE type, glm::vec3* v1, glm::vec3* v2,
		glm::vec3* v3, unsigned int startIndex);

public:

	std::string Name;
	std::string modelPath;
	std::vector<Texture*> ExtraTextures;

	void CreateSimpleVertex();

	void Parse2DQuad();
	void Parse2DCircle();

	void ParseOBJ(std::string fileName);
	//void ParseFBX(std::string fileName);



	int GetSectionsToDraw() { return sections; }
	std::vector<int> GetVertexPerSection() { return sectionVertex; }
	std::vector<std::string> GetTextureNames() { return textureNames; }
	std::unordered_map<std::string, int> GetTextureMap() { return texturesMap; }
	std::vector<int> getIndexBufferData() { return indexBufferData; }
	std::vector<Texture*> getModelTextures() { return textures; }
	int getIndexBufferSize() { return indexBufferSize; }
	


	VertexData* GetVertexData() { return coordinatesData; }
	VertexData* GetTexturesData() { return textureData; }
	VertexData* GetNormalsData() { return normalData; }
	VertexData* GetTangentsData() { return tangentData; }
	VertexData* GetColorData() { return colorsData; }

	void getStructData(
		std::istringstream& contentStream,
		std::string& line,
		VertexData* structType);


	ModelLoader() : 
		tempCoordinatesData(new VertexData), 
		tempTextureData(new VertexData), 
		tempNormalData(new VertexData),
		coordinatesData(new VertexData),
		textureData(new VertexData),
		normalData(new VertexData),
		tangentData(new VertexData) {}
	
	~ModelLoader() 
	{
		delete tempCoordinatesData;
		delete tempTextureData;
		delete tempNormalData;
		delete coordinatesData;
		delete textureData;
		delete normalData;
		delete tangentData;
	}
};



