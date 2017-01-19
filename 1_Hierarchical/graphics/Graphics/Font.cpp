#include "Font.h"





Font::Font()
{
}

Font::~Font()
{
}

bool Font::Initialize(std::string fontFile)
{
	this->fontFile = fontFile;
	return true;
	/*if (TTF_Init() != 0)
	{
		printf("Error initializing ttf");
		return false;
	}
	return true;*/
}

bool Font::Load(int pointSize)
{
	font = TTF_OpenFont(fontFile.c_str(), pointSize);
	if (font == NULL)
	{
		printf("Error loading ttf %s", TTF_GetError());
		return false;
	}
	return true;
}

void Font::Free()
{
	TTF_CloseFont(font);
}

TTF_Font* Font::GetFont()
{
	return font;
}