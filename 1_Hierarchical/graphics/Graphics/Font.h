#pragma once

#include <SDL_ttf.h>
#include <iostream>

class Font
{
private:
	TTF_Font* font;
	std::string fontFile;
	const char* text;

public:
	Font();
	~Font();

	bool Initialize(std::string fontFile);
	bool Load(int pointSize);
	void Free();
	TTF_Font* GetFont();
	const char* GetText() { return text; }

};

