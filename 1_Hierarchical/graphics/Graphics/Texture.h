#pragma once
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <glew.h>
#include <string>
#include "Font.h"

class Texture
{

private:
	std::string filePath;
	void initializeDataPath();
	GLuint textureId;
	SDL_Surface* image;
	Font* font;

public:
	GLuint Create();
	void CreateFromText(std::string text, SDL_Color textColor);
	void Use();
	void Free();
	void Destroy();

	void SetFont(Font* font) { this->font = font; }
	Font* GetFont() { return font; }

	GLuint GetId() { return textureId; }
	std::string GetPath() { return filePath; }

	void SetImage(SDL_Surface image) { this->image = &image; }
	SDL_Surface GetImage() 
	{ 
		return *image; 
	}

	Texture() {} 
	Texture(std::string fileName) : filePath(fileName) 
	{
		initializeDataPath();
	}

	~Texture() 
	{
		glDeleteTextures(1, &textureId);
	}
};

