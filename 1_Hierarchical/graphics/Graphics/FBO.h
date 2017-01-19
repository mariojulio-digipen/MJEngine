#pragma once


class FBO
{

private:
	unsigned int fbo;
	unsigned int texture;
	int width;
	int height;

public:
	FBO() {}
	~FBO() {}

	void Create(const int w, const int h);
	void Bind();
	void UnBind();
};

