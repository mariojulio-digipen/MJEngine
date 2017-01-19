#include "Shader.h"
#include "Transform.h"
#include <SDL.h>


#include <sstream>
#include <fstream>
#include <iostream>


Shader::Shader() : 
	VertexAttLocation(0), 
	TextureAttLocation(1), 
	NormalAttLocation(2),
	TangetAttLocation(3),
	BoneIdAttLocation(4),
	BoneWeightAttLocation(5)
{
}
//
//Shader::Shader(RENDER_TYPE type)
//{
//	this->renderType = type;
//}

Shader::~Shader()
{
	CleanUp();
}

void Shader::SetColor(glm::vec4 color)
{
	GLint colorLoc = glGetUniformLocation(shaderProgram, "NewColor");
	glUniform4fv(colorLoc, 1, &color[0]);
}

void Shader::ChooseTextureUnit(const int textureUnit, std::string unitName)
{
	int pid;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pid);
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	GLint loc = glGetUniformLocation(pid, unitName.c_str());
	glUniform1i(loc, textureUnit);
}

void Shader::ReleaseTextureUnit()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Shader::UseObjectShadingSection(int section)
{
	GLint ShadingSectionLoc = glGetUniformLocation(this->GetProgramID(), "SuportedTexture");
	glUniform1i(ShadingSectionLoc, section);
}

void Shader::ReleaseShadingSection()
{
	GLint ShadingSectionLoc = glGetUniformLocation(this->GetProgramID(), "SuportedTexture");
	glUniform1i(ShadingSectionLoc, -1);
}

void Shader::SetTextCanvasBackGround(int value)
{
	GLint isText = value;
	GLint ISText = glGetUniformLocation(this->GetProgramID(), "IsText");
	glUniform1i(ISText, isText);

}

void Shader::SetTransparency(int value)
{
	GLint isTransparent = value;
	GLint transparencyLocation = glGetUniformLocation(this->GetProgramID(), "Transparent");
	glUniform1i(transparencyLocation, isTransparent);
}

void Shader::SetLightsOn()
{
	GLint is2D = 1;
	GLint _2DLoc = glGetUniformLocation(this->GetProgramID(), "Lights");
	glUniform1i(_2DLoc, is2D);
}

void Shader::SetLightsOff()
{
	GLint is2D = 0;
	GLint _2DLoc = glGetUniformLocation(this->GetProgramID(), "Lights");
	glUniform1i(_2DLoc, is2D);
}

void Shader::SetDebugModeOn()
{
	GLint isDebug = 1;
	GLint debugLoc = glGetUniformLocation(this->GetProgramID(), "DebugMode");
	glUniform1i(debugLoc, isDebug);
}

void Shader::SetDebugModeOff()
{
	GLint isDebug = 0;
	GLint debugLoc = glGetUniformLocation(this->GetProgramID(), "DebugMode");
	glUniform1i(debugLoc, isDebug);
}

void Shader::HasExtraTextures(bool has)
{
	GLboolean hasTextures = has;
	GLint hasTexturesLoc = glGetUniformLocation(this->GetProgramID(), "HasExtraTextures");
	glUniform1i(hasTexturesLoc, hasTextures);
}

void Shader::IsExtraActive(bool is)
{
	GLboolean ExtraTexActive = is;
	GLint ExtraTexActiveLoc = glGetUniformLocation(this->GetProgramID(), "IsExtraActive");
	glUniform1i(ExtraTexActiveLoc, ExtraTexActive);
}

void Shader::Enable2D()
{
	//renderType = R2D;
	SetLightsOff();
}

void Shader::Disable2D()
{
	//renderType = R3D;
	SetLightsOn();
}

std::string Shader::ReadFile(const char* file)
{
	std::ifstream stream(file);
	std::stringstream buffer;
	buffer << stream.rdbuf();
	std::string fileContent = buffer.str();

	return fileContent;
}

void Shader::BindAttributeLocation(int index, const std::string &attribute)
{
	glBindAttribLocation(shaderProgram, index, attribute.c_str());
}

void Shader::UseProgram()
{
	glUseProgram(shaderProgram);
}

void Shader::UnUseProgram()
{
	glUseProgram(0);
}

bool Shader::Init(std::string vertexFile, std::string fragmentFile)
{
	
	shaderProgram = glCreateProgram();

	BindAttributeLocation(VertexAttLocation, "Position");
	BindAttributeLocation(TextureAttLocation, "TextureCoord");
	BindAttributeLocation(NormalAttLocation, "Normal");
	BindAttributeLocation(TangetAttLocation, "Tangent");
	BindAttributeLocation(BoneIdAttLocation, "BoneIDs");
	BindAttributeLocation(BoneWeightAttLocation, "BoneWeights");

	if (!LoadVertexShader(vertexFile))
		return false;

	if (!LoadFragmentShader(fragmentFile))
		return false;

	return LinkShaders();
}

int Shader::GetProgramID()
{
	return shaderProgram;
}

bool Shader::LoadVertexShader(const std::string &fileName)
{
	std::cout << "Loading Vertex shader" << std::endl;

	std::string str = ReadFile(fileName.c_str());

	char* src = const_cast<char*>(str.c_str());
	int size = str.length();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &src, &size);
	glCompileShader(vertexShader);

	int success = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		PrintShaderCompilationErrorInfo(vertexShader);
		return false;
	}

	glAttachShader(shaderProgram, vertexShader);
	return true;
}

bool Shader::LoadFragmentShader(const std::string &fileName)
{
	std::cout << "Loading Fragment shader" << std::endl;

	std::string str = ReadFile(fileName.c_str());

	char* src = const_cast<char*>(str.c_str());
	int size = str.length();

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &src, &size);
	glCompileShader(fragmentShader);

	int success = 0;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		PrintShaderCompilationErrorInfo(fragmentShader);
		return false;
	}

	glAttachShader(shaderProgram, fragmentShader);
	return true;
}

bool Shader::LinkShaders()
{
	glLinkProgram(shaderProgram);

	int success = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (success == false)
		PrintShaderLinkingError(shaderProgram);

	return success != 0;
}

void Shader::PrintShaderLinkingError(int shaderId)
{
	std::cout << "=======================================\n";
	std::cout << "Shader linking failed : " << std::endl;

	// Find length of shader info log
	int maxLength;
	glGetProgramiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

	std::cout << "Info Length : " << maxLength << std::endl;

	// Get shader info log
	char* shaderProgramInfoLog = new char[maxLength];
	glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, shaderProgramInfoLog);

	std::cout << "Linker error message : " << shaderProgramInfoLog << std::endl;

	/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
	/* In this simple program, we'll just leave */
	delete shaderProgramInfoLog;
	return;
}

void Shader::PrintShaderCompilationErrorInfo(int shaderId)
{
	std::cout << "=======================================\n";
	std::cout << "Shader compilation failed : " << std::endl;

	// Find length of shader info log
	int maxLength;
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

	// Get shader info log
	char* shaderInfoLog = new char[maxLength];
	glGetShaderInfoLog(shaderId, maxLength, &maxLength, shaderInfoLog);

	// Print shader info log
	std::cout << "\tError info : " << shaderInfoLog << std::endl;

	std::cout << "=======================================\n\n";
	delete shaderInfoLog;
}

void Shader::CleanUp()
{
	glUseProgram(0);
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);
}


