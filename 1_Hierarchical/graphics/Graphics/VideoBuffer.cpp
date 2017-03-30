#include "VideoBuffer.h"

#include <glew.h>

void VideoBuffer::save(bool video)
{
	static int i = 0;
	std::stringstream out;
	if (video) {
		if (i < 10)
			out << "C:\\movies\\video000" << (i++) << ".bmp";
		else if (i < 100)
			out << "C:\\movies\\video00" << (i++) << ".bmp";
		else if (i < 1000)
			out << "C:\\movies\\video0" << (i++) << ".bmp";
		else if (i < 10000)
			out << "C:\\movies\\video" << (i++) << ".bmp";
	}
	else {
		out << "C:\\movies\\capture" << (i++) << ".bmp";
	}
	std::string s = out.str();

	glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, buffer);

	std::fstream of(s.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	char header[18] = { 0 };
	header[2] = 2;
	header[12] = width & 0xff;
	header[13] = width >> 8;
	header[14] = height & 0xff;
	header[15] = height >> 8;
	header[16] = 32;

	of.write(header, 18);
	of.write((char *)buffer, width * height * 4);
}