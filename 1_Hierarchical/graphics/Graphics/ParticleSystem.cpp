#include "ParticleSystem.h"
#include "Texture.h"
#include "ModelLoader.h"

extern Shader* meshParticleShader;

ParticleSystem::ParticleSystem() :
	maxParticles(0),
	numParticles(0),
	vertexAttributeIndex(0),
	textureAttributeIndex(1),
	normalAttributeIndex(2),
	instanceMatrixAttributeIndex(6),
	//positionSizeAttributeIndex(3),
	//colorAttributeIndex(4),
	vertexDataNumber(0),
	textureDataNumber(0),
	normalDataNumber(0),
	//vertexData(nullptr),
	//textureData(nullptr),
	//normalData(nullptr),
	//modelMatrices(nullptr),
	texture(nullptr),
	contextReady(true)
{

}

ParticleSystem::~ParticleSystem()
{
	//delete[] vertexData;
	//delete[] textureData;
	//delete[] normalData;
	//delete[] modelMatrices; 
	//delete gpuParticlePosSizeData;

	delete texture;

	deleteGLBuffers();
}

//int ParticleSystem::GetShaderUsed()
//{ 
//	return meshParticleShader->GetProgramID(); 
//}

void ParticleSystem::Init()
{
	createTexture();
	//gpuParticlePosSizeData = new GLfloat[maxParticles];
	//gpuParticleColorData = new GLfloat[maxParticles];

	//modelMatrices = new glm::mat4[maxParticles];
	modelMatrices.resize(maxParticles);

	// data on CPU
	fillGPUParticleData();

	// send to GPU
	createGLBuffers();
	buildParticleVBOs();
	bindAttributes();

	
	
}

void ParticleSystem::SetTextureImage(const char * fileName)
{
	textureFile = "ArtAssets\\" + std::string(fileName);
}

void ParticleSystem::createTexture()
{
	//texture = new Texture("ArtAssets\\piso.png");
	texture = new Texture(textureFile);
	texture->Create();
}

void ParticleSystem::fillGPUParticleData()
{
	// load the model
	// it's ok obj, usually for particles the models 
	// will be very simple
	ModelLoader loader;
	loader.ParseOBJ("ArtAssets\\exported\\target.obj");

	// store the model geometry information in the Data vars
	// first get number of data for vertexData and the data itself
	VertexData* vertex = loader.GetVertexData();
	vertexDataNumber = vertex->numberOfData;
	//vertexData = new GLfloat[vertexDataNumber];
	vertexData.resize(vertexDataNumber);
	std::memcpy(&vertexData[0], &vertex->dataArray[0], vertexDataNumber * sizeof(float));
	//vertexData = &vertex->dataArray[0];
	/*for (int i = 0; i < vertexDataNumber; i++)
		vertexData[0] = vertex->dataArray[0];*/


	VertexData* textured = loader.GetTexturesData();
	textureDataNumber = textured->numberOfData;
	//textureData = new GLfloat[textureDataNumber];
	textureData.resize(textureDataNumber);
	std::memcpy(&textureData[0], &textured->dataArray[0], textureDataNumber * sizeof(float));
	//textureData = &texture->dataArray[0];
	/*for (int i = 0; i < textureDataNumber; i++)
		textureData[0] = texture->dataArray[0];*/

	VertexData* normal = loader.GetNormalsData();
	normalDataNumber = normal->numberOfData;
	//normalData = new GLfloat[normalDataNumber];
	normalData.resize(normalDataNumber);
	std::memcpy(&normalData[0], &normal->dataArray[0], normalDataNumber * sizeof(float));
	//normalData = &normal->dataArray[0];
	/*for (int i = 0; i < normalDataNumber; i++)
		normalData[0] = normal->dataArray[0];*/



}

void ParticleSystem::createGLBuffers()
{
	
	glGenVertexArrays(1, &eachParticleVAO);

	// shared
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &textureVBO);
	glGenBuffers(1, &normalVBO);

	// instanced
	glGenBuffers(1, &modelMatrixVBO);
	//glGenBuffers(1, &positionSizeVBO);
	//glGenBuffers(1, &colorVBO);

	//GLenum err = glGetError();
}

void ParticleSystem::deleteGLBuffers()
{
	// delete vbos first
	// shared
	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &textureVBO);
	glDeleteBuffers(1, &normalVBO);

	// instanced
	glDeleteBuffers(1, &modelMatrixVBO);
	//glDeleteBuffers(1, &positionSizeVBO);
	//glDeleteBuffers(1, &colorVBO);

	// finally delete the vao
	glDeleteVertexArrays(1, &eachParticleVAO);
}

void ParticleSystem::buildParticleVBOs()
{
	int sizeInBytes = 0;

	
	// shared attribute
	// bind the VAO where the shared attributes will be stored
	glBindVertexArray(eachParticleVAO);

	// vertexVBO
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	sizeInBytes = vertexDataNumber * sizeof(GLfloat);
	glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &vertexData[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertexAttributeIndex);
	glVertexAttribPointer(vertexAttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// textureVBO
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	sizeInBytes = textureDataNumber * sizeof(GLfloat);
	glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &textureData[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(textureAttributeIndex);
	glVertexAttribPointer(textureAttributeIndex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// normalVBO
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	sizeInBytes = normalDataNumber * sizeof(GLfloat);
	glBufferData(GL_ARRAY_BUFFER, sizeInBytes, &normalData[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(normalAttributeIndex);
	glVertexAttribPointer(normalAttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	// individual attributes
	glBindBuffer(GL_ARRAY_BUFFER, modelMatrixVBO);
	sizeInBytes = maxParticles * sizeof(glm::mat4); // initialize the whole memory (max)
	// initialize null
	glBufferData(GL_ARRAY_BUFFER, sizeInBytes, NULL, GL_STREAM_DRAW); // this is the key


	//// color
	//glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	//sizeInBytes = maxParticles * 4 * sizeof(GLubyte); // GLubyte = unsigned char
	//// initialize null
	//glBufferData(GL_ARRAY_BUFFER, sizeInBytes, NULL, GL_STREAM_DRAW); // this is the key

	glBindVertexArray(0);
}

void ParticleSystem::bindAttributes()
{
	GLsizei vec4Size = sizeof(glm::vec4);
	GLsizei mat4Size = 4 * vec4Size;

	int c1 = instanceMatrixAttributeIndex;
	int c2 = instanceMatrixAttributeIndex + 1;
	int c3 = instanceMatrixAttributeIndex + 2;
	int c4 = instanceMatrixAttributeIndex + 3;
	
	glBindVertexArray(eachParticleVAO);

		int sizeInBytes = 0;
		// model matrix
		glBindBuffer(GL_ARRAY_BUFFER, modelMatrixVBO);
		sizeInBytes = maxParticles * sizeof(glm::mat4);
		glBufferData(GL_ARRAY_BUFFER, sizeInBytes, NULL, GL_STREAM_DRAW); // this is the key

		// matrix column 1
		glEnableVertexAttribArray(c1); // 6
		glVertexAttribPointer(
			c1, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)0);
		// matrix column 2
		glEnableVertexAttribArray(c2); // 7
		glVertexAttribPointer(
			c2, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(vec4Size));
		// matrix column 3
		glEnableVertexAttribArray(c3); // 8
		glVertexAttribPointer(
			c3, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(2 * vec4Size));
		// matrix column 4
		glEnableVertexAttribArray(c4); // 9
		glVertexAttribPointer(
			c4, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(3 * vec4Size));

		glVertexAttribDivisor(c1, 1);
		glVertexAttribDivisor(c2, 1);
		glVertexAttribDivisor(c3, 1);
		glVertexAttribDivisor(c4, 1);

	glBindVertexArray(0);


	//// position and size
	//glEnableVertexAttribArray(positionSizeAttributeIndex);
	//glBindBuffer(GL_ARRAY_BUFFER, positionSizeVBO);
	//glVertexAttribPointer(
	//	positionSizeAttributeIndex, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//// color
	//glEnableVertexAttribArray(colorAttributeIndex);
	//glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	//glVertexAttribPointer(
	//	colorAttributeIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);

}

void ParticleSystem::updateVBOs()
{
	int sizeInBytes = 0;

	// instance matrix update
	glBindBuffer(GL_ARRAY_BUFFER, modelMatrixVBO);
	sizeInBytes = maxParticles * sizeof(glm::mat4); 
	// Buffer orphaning, a common way to improve streaming performance
	glBufferData(GL_ARRAY_BUFFER, sizeInBytes, NULL, GL_STREAM_DRAW);
	sizeInBytes = numParticles * sizeof(glm::mat4); // updates with the actual number
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeInBytes, &modelMatrices[0]);

	// pos and size
	//glBindBuffer(GL_ARRAY_BUFFER, positionSizeVBO);
	//sizeInBytes = maxParticles * 4 * sizeof(float);
	// Buffer orphaning, a common way to improve streaming performance
	//glBufferData(GL_ARRAY_BUFFER, sizeInBytes, NULL, GL_STREAM_DRAW);
	//sizeInBytes = numParticles * 4 * sizeof(float); // updates with the actual number
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeInBytes, gpuParticlePosSizeData);


	//glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	//sizeInBytes = maxParticles * 4 * sizeof(GLubyte);
	//// Buffer orphaning, a common way to improve streaming performance
	//glBufferData(GL_ARRAY_BUFFER, sizeInBytes, NULL, GL_STREAM_DRAW);
	//sizeInBytes = numParticles * 4 * sizeof(GLubyte);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeInBytes, gpuParticleColorData);

	
}

void ParticleSystem::drawParticle()
{
	glBindVertexArray(eachParticleVAO);

		meshParticleShader->UseProgram();
		updateVBOs();
		//
		/*int pos = 
			glGetAttribLocation(meshParticleShader->GetProgramID(), "Position");
		int tex =
			glGetAttribLocation(meshParticleShader->GetProgramID(), "TextureCoord");
		int nor =
			glGetAttribLocation(meshParticleShader->GetProgramID(), "Normal");
		int tan =
			glGetAttribLocation(meshParticleShader->GetProgramID(), "Tangent");
		int b1 =
			glGetAttribLocation(meshParticleShader->GetProgramID(), "BoneIDs");
		int b2 =
			glGetAttribLocation(meshParticleShader->GetProgramID(), "BoneWeights");
		int instance = 
			glGetAttribLocation(meshParticleShader->GetProgramID(), "InstanceMatrix");

		int other1 = 
			glGetAttribLocation(meshParticleShader->GetProgramID(), "LightPositionIn");
		int other2 =
			glGetAttribLocation(meshParticleShader->GetProgramID(), "LightAmbientIn");
		int other3 =
			glGetAttribLocation(meshParticleShader->GetProgramID(), "LightIntensityIn");
		int other4 =
			glGetAttribLocation(meshParticleShader->GetProgramID(), "LightColorIn");*/


		glVertexAttribDivisor(vertexAttributeIndex, 0);
		glVertexAttribDivisor(textureAttributeIndex, 0);
		glVertexAttribDivisor(normalAttributeIndex, 0);
		//glVertexAttribDivisor(colorAttributeIndex, 1); // different color each

		//glVertexAttribDivisor(positionSizeAttributeIndex, 1); // different pos/size each

		glEnable(GL_DEPTH_TEST);

		meshParticleShader->ChooseTextureUnit(0, "textureSampler");
		texture->Use();

		glDrawArraysInstanced(GL_TRIANGLES, 0, vertexDataNumber/3, numParticles);

		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, 0);

		meshParticleShader->UnUseProgram();

	glBindVertexArray(0);
}

void ParticleSystem::Draw()
{
	drawParticle();
}


void ParticleSystem::SetModelMatrices(glm::mat4* modelMatrices)
{
	std::memcpy(&this->modelMatrices[0], modelMatrices, maxParticles * sizeof(glm::mat4));
}

void ParticleSystem::SetModelMatrices(glm::mat4 * modelMatrices, int amount)
{
	std::memcpy(&this->modelMatrices[0], modelMatrices, amount * sizeof(glm::mat4));
}

std::vector<glm::mat4> ParticleSystem::GetModelMatrices()
{
	return modelMatrices;
}

float ParticleSystem::SpiralRotationAngle(float sigma, float timeStep)
{
	return sigma * timeStep;
}

float ParticleSystem::VortexRotationAngle(glm::vec3 xt, glm::vec3 vo, glm::vec3 u, float K, float T, float timeStep)
{
	float numerator = K*timeStep;

	float magnitude = glm::length(xt - vo);
	float dot = glm::dot(u, xt - vo);
	float denominator_base = magnitude * magnitude - dot * dot;
	float denominator = glm::pow(denominator_base, T*0.5f);

	float angle = numerator / denominator;
	return angle;
}

glm::mat3 ParticleSystem::SpiralRotationMatrix(float angle, glm::vec3 u)
{
	glm::mat3 R;
	glm::mat3 R_1;
	glm::mat3 R_2;
	glm::mat3 R_3;
	R_1 *= glm::cos(angle);

	u = glm::vec3(0.0f, 1.0f, 0.0f);
	u = glm::normalize(u);
	glm::mat3 outer = glm::outerProduct(u, u);
	R_2 = (1 - glm::cos(angle)) * outer;


	glm::vec3 c1 = glm::vec3(0.0f, -u.z, u.y);
	glm::vec3 c2 = glm::vec3(u.z, 0.0f, -u.x);
	glm::vec3 c3 = glm::vec3(-u.y, u.x, 0.0f);
	glm::mat3 hat_u = glm::mat3(c1, c2, c3);

	R_3 = glm::sin(angle) * hat_u;

	R = R_1 + R_2 + R_3;

	return R;
}