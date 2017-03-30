#include "Texture.h"
#include "Font.h"
#include <algorithm>
#include <SDL.h>
#include <SDL_ttf.h>




void Texture::initializeDataPath() 
{
	std::string relativePath(SDL_GetBasePath());
	/*std::string::iterator it = relativePath.end() - 2;
	for (; it >= relativePath.begin(); it--)
	{
		if ((*it) == '\\')
			break;
	}
	relativePath.erase(it, relativePath.end()-1);*/
	filePath = relativePath + filePath;
}

GLuint Texture::Create()
{
	/*SDL_RWops* rwop;
	rwop = SDL_RWFromFile(filePath.c_str(), "rb");*/
	image = IMG_Load(filePath.c_str());
	//image = IMG_LoadPNG_RW(rwop);
	if (!image)
		printf("There was an error while creating image for texture. Error: %s\n",
			IMG_GetError());

	
	glGenTextures(1, &textureId);
	GLenum a =glGetError();

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		image->w, image->h,
		0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	return textureId;
}

void Texture::CreateFromText(std::string text, SDL_Color textColor)
{
	
	font->Load(48);

	glDeleteTextures(1, &textureId);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	SDL_Surface* textSurface = TTF_RenderText_Blended(
		font->GetFont(), text.c_str(), textColor);

	image = SDL_ConvertSurfaceFormat(textSurface, SDL_PIXELFORMAT_ABGR8888, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		image->w, image->h,
		0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	
}

void Texture::Use()
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	
}

void Texture::Free()
{
}

void Texture::Destroy()
{
}
