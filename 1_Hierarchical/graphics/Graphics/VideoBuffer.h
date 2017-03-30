#pragma once

#include <sstream>
#include <fstream>


class VideoBuffer
{
private:
	int width;
	int height;
	unsigned char* buffer;

public:
	VideoBuffer(const int w, const int h) : 
		width(w), 
		height(h), 
		buffer(new unsigned char[w*h*4]){}


	~VideoBuffer() 
	{
		if (buffer)
			delete[] buffer;
	}


	void save(bool video);
};

