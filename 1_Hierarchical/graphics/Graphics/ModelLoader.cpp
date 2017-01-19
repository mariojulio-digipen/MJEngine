#include "ModelLoader.h"
#include <iostream>
#include <fstream>
#include <SDL.h>
#include <cmath> 
#include <algorithm>
#include <ctype.h>
#include <glm.hpp>


void ModelLoader::initializeModelPath(std::string fileName)
{
	std::string relativePath(SDL_GetBasePath());

	//std::cout << "\nrelative path\n: " << relativePath;
	modelPath = relativePath + fileName;
}


void ModelLoader::CreateSimpleVertex()
{
	for (int i = 0; i < 3; i++)
	{
		coordinatesData->dataArray.push_back(vertex[i]);
		coordinatesData->floatsPerData = 3;
		coordinatesData->numberOfData = coordinatesData->dataArray.size();
	}
		
	sectionVertex.push_back(coordinatesData->numberOfData / 3);
	this->sections = 1;
}

void ModelLoader::Parse2DQuad()
{
	for (int i = 0; i < 8 * 3; i++)
	{
		coordinatesData->dataArray.push_back(quad2D[i]);
		coordinatesData->floatsPerData = 3;
		coordinatesData->numberOfData = coordinatesData->dataArray.size();
	}
	sectionVertex.push_back(coordinatesData->numberOfData/3);
}

void ModelLoader::Parse2DCircle()
{
	coordinatesData->dataArray.push_back(1.0f);
	coordinatesData->dataArray.push_back(0.0f);
	coordinatesData->dataArray.push_back(0.0f);


	int slices = 10;
	int n = slices * 4;
	float angleSlice = 90.0f / slices;
	for (int i = 1; i < n; i++)
	{
		coordinatesData->dataArray.push_back(cosf(glm::radians(i*angleSlice)));
		coordinatesData->dataArray.push_back(sinf(glm::radians(i*angleSlice)));
		coordinatesData->dataArray.push_back(0.0f);
		coordinatesData->dataArray.push_back(cosf(glm::radians(i*angleSlice)));
		coordinatesData->dataArray.push_back(sinf(glm::radians(i*angleSlice)));
		coordinatesData->dataArray.push_back(0.0f);
	}
	coordinatesData->dataArray.push_back(1.0f);
	coordinatesData->dataArray.push_back(0.0f);
	coordinatesData->dataArray.push_back(0.0f);





	coordinatesData->floatsPerData = 3;
	coordinatesData->numberOfData = coordinatesData->dataArray.size();

	sectionVertex.push_back(coordinatesData->numberOfData/3);

	int a = 10;
}

void ModelLoader::getStructData(
	std::istringstream& contentStream,
	std::string& line,
	VertexData* structType)
{
	std::getline(contentStream, line);
	std::istringstream lineStream(line);
	std::string value;
	while (lineStream >> value)
	{
		float actualValue = std::stof(value.c_str());
		structType->dataArray.push_back(actualValue);
	}
}

void ModelLoader::parseMaterial(std::string fileName)
{
	fileName.erase(std::remove_if(fileName.begin(), fileName.end(), isspace), fileName.end());
	initializeModelPath("ArtAssets\\exported\\" + fileName);
	//modelPath = "Graphics\\ArtAssets\\exported\\" + fileName;
	std::string content = readFile(modelPath).str();
	
	std::istringstream contentStream(content.c_str());
	std::string word;
	std::string line;

	int id = 0;
	while (contentStream >> word)
	{
		if (word == "#")
		{
			std::getline(contentStream, line);
			continue;
		}

		if (word == "newmtl")
		{
			std::string tmpWord;
			while (contentStream >> word)
			{
				if(word.find("___Default") != std::string::npos)
					tmpWord = word;

				std::string lineProp;
				std::string wordProp;
				if (word == "map_Kd")
				{
					std::string materialFileName;
					std::getline(contentStream, materialFileName);
					materialFileName.erase(
						std::remove_if(materialFileName.begin(),
							materialFileName.end(), isspace),
						materialFileName.end()
					);
					//textureNames.push_back(tmpWord);
					Texture* texture = new Texture("ArtAssets\\" + materialFileName);
					textures.push_back(texture);
					
					std::unordered_map<std::string, int>::const_iterator it = texturesMap.find(tmpWord);
					int mytexId;
					if (it != texturesMap.end())
						mytexId = it->second;
					else
						mytexId = id;
					texturesMap.insert(std::make_pair(tmpWord, ++id));
					continue;
				}
				if (word == "support")
				{
					std::string extras;
					std::getline(contentStream, extras);
					while (contentStream >> line)
					{
						if (line == "endsupport")
							break;
						std::string extraMaterial;
						std::getline(contentStream, extraMaterial);
						extraMaterial.erase(
							std::remove_if(extraMaterial.begin(),
								extraMaterial.end(), isspace),
							extraMaterial.end()
						);
						Texture* texture = new Texture("ArtAssets\\" + extraMaterial);
						ExtraTextures.push_back(texture);
					}
				}
			}
		}

		int a = 1;
	}
}

void ModelLoader::ParseOBJ(std::string fileName)
{
	initializeModelPath(fileName);
	fileContent = readFile(modelPath).str();
	std::istringstream contentStream(fileContent.c_str());
	std::string word;
	std::string line;
	int indexDataCount = 0;

	int x, y, z;
	int vertexIndex, textureIndex, normalIndex;
	float vDataX, vDataY, vDataZ;
	float tDataU, tDataV;
	float nDataX, nDataY, nDataZ;
	unsigned int offset = 3;
	float scale;
	bool isFont = false;

	triangleCountFromFile = 0;
	while (contentStream >> word)
	{
		if (word == "#")
		{
			std::getline(contentStream, line);
			continue;
		}
		if (word == "textcanvas")
		{
			isFont = true;
			std::getline(contentStream, line);
			line.erase(
				std::remove_if(line.begin(),
					line.end(), isspace),
				line.end()
			);
			
			Texture* texture = new Texture;
			Font* font = new Font;
			font->Initialize(line);
			texture->SetFont(font);

			textures.push_back(texture);

		}
		if (word == "mtllib")
		{
			isFont = false;
			std::getline(contentStream, line);
			materialLibs.push_back(line);
			continue;
		}
		if (word == "v")
		{
			getStructData(contentStream, line, tempCoordinatesData);
			continue;
		}
		if (word == "vn")
		{
			getStructData(contentStream, line, tempNormalData);
			continue;
		}
		if (word == "vt")
		{
			getStructData(contentStream, line, tempTextureData);
			continue;
		}
		
		if (word.find("___Default") != std::string::npos)
		{
			//sectionsId.push_back(sections++);
			sections++;
			textureNames.push_back(word);
			std::getline(contentStream, line);
			std::istringstream lineStream(line);
			std::string word;
			while (contentStream >> word)
			{
				if (word == "usemtl")
					break;

				if (word == "f")
				{
					triangleCountFromFile++;
					std::getline(contentStream, line);
					std::istringstream lineStream(line);
					std::string value;
					while (lineStream >> value)
					{
						coordsIndexData.push_back(0);
						textureIndexData.push_back(0);
						normalIndexData.push_back(0);
						sscanf_s(value.c_str(), "%i/%i/%i",
							&coordsIndexData[indexDataCount],
							&textureIndexData[indexDataCount],
							&normalIndexData[indexDataCount]);
						indexDataCount++;
						vertexCounter++;
					}
					printf("");
				}
			}
			sectionVertex.push_back(vertexCounter);
			vertexCounter = 0;
		}


		
	}




	for (unsigned int i = 0; i < 3; i++)
	{
		vertexIndex = coordsIndexData[i] - 1;
		x = vertexIndex * offset;
		y = vertexIndex * offset + 1;
		z = vertexIndex * offset + 2;
		vDataX = tempCoordinatesData->dataArray[x];
		vDataY = tempCoordinatesData->dataArray[y];
		vDataZ = tempCoordinatesData->dataArray[z];
		if (vDataX != 0.0f)
		{
			scale = abs(vDataX);
			break;
		}
		else if (vDataY != 0.0f)
		{
			scale = abs(vDataY);
			break;
		}
		else if (vDataZ != 0.0f)
		{
			scale = abs(vDataZ);
			break;
		}
	}
	for (unsigned int i = 0; i < coordsIndexData.size(); i++)
	{
		vertexIndex = coordsIndexData[i] - 1;
		x = vertexIndex * offset;
		y = vertexIndex * offset + 1;
		z = vertexIndex * offset + 2;

		vDataX = tempCoordinatesData->dataArray[x];
		vDataY = tempCoordinatesData->dataArray[y];
		vDataZ = tempCoordinatesData->dataArray[z];
		if (vDataX != 0.0f) coordinatesData->dataArray.push_back((vDataX / scale) / 2);
		else coordinatesData->dataArray.push_back(0.0f);
		if (vDataY != 0.0f) coordinatesData->dataArray.push_back((vDataY / scale) / 2);
		else coordinatesData->dataArray.push_back(0.0f);
		if (vDataZ != 0.0f) coordinatesData->dataArray.push_back((vDataZ / scale) / 2);
		else coordinatesData->dataArray.push_back(0.0f);
	}
	for (unsigned int i = 0; i < textureIndexData.size(); i++)
	{
		textureIndex = textureIndexData[i] - 1;
		x = textureIndex * offset;
		y = textureIndex * offset + 1;

		tDataU = tempTextureData->dataArray[x];
		tDataV = tempTextureData->dataArray[y];

		textureData->dataArray.push_back(tDataU);
		textureData->dataArray.push_back(tDataV);
	}

	for (unsigned int i = 0; i < normalIndexData.size(); i++)
	{
		normalIndex = normalIndexData[i] - 1;
		x = normalIndex * offset;
		y = normalIndex * offset + 1;
		z = normalIndex * offset + 2;

		nDataX = tempNormalData->dataArray[x];
		nDataY = tempNormalData->dataArray[y];
		nDataZ = tempNormalData->dataArray[z];

		normalData->dataArray.push_back(nDataX);
		normalData->dataArray.push_back(nDataY);
		normalData->dataArray.push_back(nDataZ);

		/*printf("vn[%i]  %f  %f  %f \n", normalIndex, nDataX, nDataY, nDataZ);
		if (i == normalIndexData.size() - 1)
			printf("\n\n\n");*/
	}
	// useful info just in case
	coordinatesData->floatsPerData = coordinatesData->dataArray.size() / coordsIndexData.size();
	coordinatesData->numberOfData = coordinatesData->dataArray.size();

	textureData->floatsPerData = textureData->dataArray.size() / textureIndexData.size();
	textureData->numberOfData = textureData->dataArray.size();


	normalData->floatsPerData = normalData->dataArray.size() / normalIndexData.size();
	normalData->numberOfData = normalData->dataArray.size();

	// tangent computation
	computeTangents();
	

	

	if (!isFont)
	{
		parseMaterial(materialLibs[0]);
	}
		

	//printf("");
}

void ModelLoader::computeTangents()
{
	/*size_t tempUV = tempTextureData->dataArray.size() / 3;
	size_t tempCoords = tempCoordinatesData->dataArray.size() / 3;*/

	// for each triangle
	for (size_t i = 0; i < triangleCountFromFile; i++)
	{
		glm::vec3 v1, v2, v3, E1, E2;
		glm::vec3 vt1, vt2, vt3, M, W;
		glm::vec3 sDir, tDir;

		setOneTriangleCoords(VERTEX, &v1, &v2, &v3, i);
		setOneTriangleCoords(UV, &vt1, &vt2, &vt3, i);
		
		// E1//W; E2//M
		E1 = v2 - v1;
		E2 = v3 - v1;

		W = vt2 - vt1;
		M = vt3 - vt1;

		// 1 / Determinant:
		float A = 1 / (W.s*M.t - W.t*M.s);

		sDir = (M.t*E1 - M.s*E2)*A;
		tDir = (W.s*E2 - W.t*E1)*A;

		tangentsSdir.push_back(sDir);
		tangentsSdir.push_back(sDir);
		tangentsSdir.push_back(sDir);

		tangentsTdir.push_back(tDir);
		tangentsTdir.push_back(tDir);
		tangentsTdir.push_back(tDir);

		int a = 1;
	}

	// for each vertex
	size_t tangSize = tangentsTdir.size();
	size_t vertexCount = coordinatesData->numberOfData / 3;
	for (size_t i = 0; i < vertexCount; i++)
	{
		unsigned int startIndex = 3 * i;
		const glm::vec3 n = glm::vec3(
			normalData->dataArray[startIndex],
			normalData->dataArray[startIndex + 1],
			normalData->dataArray[startIndex + 2]
		);
		const glm::vec3 ts = tangentsSdir[i];

		const glm::vec3 tt = tangentsTdir[i];

		glm::vec3 tangent = (ts - n*glm::dot(n,ts));
		tangent = glm::normalize(tangent);

		tangentData->dataArray.push_back(tangent.x);
		tangentData->dataArray.push_back(tangent.y);
		tangentData->dataArray.push_back(tangent.z);
	}

	tangentsSdir.clear();
	tangentsTdir.clear();
}

void ModelLoader::setOneTriangleCoords(VBO_TYPE type, glm::vec3* v1, glm::vec3* v2, 
	glm::vec3* v3, unsigned int idx)
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

std::stringstream ModelLoader::readFile(std::string modelPath)
{
	std::string line;
	std::stringstream fileString;
	std::ifstream file(modelPath);
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			fileString << line << "\n";
		}
		file.close();
	}
	else
	{
		printf("Unable to open model file.");
	}
	return fileString;
}