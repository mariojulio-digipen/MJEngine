#pragma once
#include "glew.h"

#include <vector>


class GBuffer
{
private:
	
	GLuint gBuffer, rbodepth;
	
	GLsizei width, height;
	

	/*void generateTexture(
		GLint internalformat,
		GLint format, 
		GLuint* texture,
		GLenum type, 
		GLenum attachment);*/

public:
	GLuint position, normal, ambient, intensity;

	GBuffer();
	GBuffer(GLsizei gWidth, GLsizei gHeight);
	~GBuffer();

	//std::vector<GLenum> Attachments;

	void Create();
	void Bind();
	void UnBind();
	void ReadBuffer(GLuint textureIndex);

};

