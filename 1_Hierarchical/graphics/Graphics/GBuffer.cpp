#include "GBuffer.h"
#include <iostream>


GBuffer::GBuffer()
{
	
}

GBuffer::GBuffer(GLsizei gWidth, GLsizei gHeight) :
	width(gWidth), height(gHeight)
{
	glGenFramebuffers(1, &gBuffer);
	//Attachments.resize(3);
}

GBuffer::~GBuffer()
{
}


void GBuffer::Create()
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer); // points to my new gbuffer

	// create all the textures

	// texture for vertex position
	glGenTextures(1, &position);
	glBindTexture(GL_TEXTURE_2D, position);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 
		0, GL_RGB, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, position, 0);

	// texture for the vertex normal
	glGenTextures(1, &normal);
	glBindTexture(GL_TEXTURE_2D, normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 
		0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, normal, 0);

	// texture for diffuse color;
	glGenTextures(1, &ambient);
	glBindTexture(GL_TEXTURE_2D, ambient);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height,
		0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
		GL_TEXTURE_2D, ambient, 0);

	// texture for specular
	glGenTextures(1, &intensity);
	glBindTexture(GL_TEXTURE_2D, intensity);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height,
		0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3
		, GL_TEXTURE_2D, intensity, 0);

	GLuint attachments[4] = { 
		GL_COLOR_ATTACHMENT0, 
		GL_COLOR_ATTACHMENT1, 
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3 };

	glDrawBuffers(4, attachments);

	// render buffer object as depth buffer and check for completeness.
	glGenRenderbuffers(1, &rbodepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbodepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbodepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
}

void GBuffer::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::ReadBuffer(GLuint textureIndex)
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + textureIndex);
}