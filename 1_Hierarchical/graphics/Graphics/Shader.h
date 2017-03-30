#pragma once

#include <glew.h>
#include <string>
#include <glm.hpp>



class Shader
{
private:
	GLuint shaderProgram;
	GLuint vertexShader;
	GLuint fragmentShader; 
	//RENDER_TYPE renderType;

	bool isTranslucent;
	

public:

	int VertexAttLocation;
	int TextureAttLocation;
	int NormalAttLocation;
	int TangetAttLocation;
	int BoneIdAttLocation;
	int BoneWeightAttLocation;
	int InstanceMatrixAttLocation;

	/*int DebugVertexAttLocation;
	int DebugColorAttLocation;*/

	//RENDER_TYPE GetRenderType() { return renderType; }

	Shader();
	//Shader(RENDER_TYPE type);
	~Shader();

	std::string ReadFile(const char* file);
	void SetDebugModeOn();
	void SetDebugModeOff();
	void BindAttributeLocation(int index, const std::string &attribute);
	void UseProgram();
	void UnUseProgram();
	bool Init(std::string vertexFile, std::string fragmentFile);
	bool LoadVertexShader(const std::string &fileName);
	bool LoadFragmentShader(const std::string &fileName);
	bool LinkShaders();
	void PrintShaderLinkingError(int shaderID);
	void PrintShaderCompilationErrorInfo(int shaderID);
	void CleanUp();
	int GetProgramID();
	void SetTransparency(int value);
	void SetTextCanvasBackGround(int value);
	void Disable2D();
	void Enable2D();
	void SetLightsOn();
	void SetLightsOff();
	void SetTranslucent(bool transluscent) { this->isTranslucent = transluscent; }
	bool IsTranslucent() { return isTranslucent; }
	void SetColor(glm::vec4 color);
	void ChooseTextureUnit(int textureUnit, std::string unitName);
	void ReleaseTextureUnit();
	void UseObjectShadingSection(int section);
	void ReleaseShadingSection();
	void HasExtraTextures(bool has);
	void IsExtraActive(bool is);
};

