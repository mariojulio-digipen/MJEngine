#pragma once

#include <glew.h>

class Render2D;
class Render3D;
class Shader;

class RenderManager
{
private:
	Render2D* render2D;
	Render3D* render3D;

public:
	RenderManager();
	~RenderManager();

	Render2D* GetRender2D() const { return render2D; }
	Render3D* GetRender3D() const { return render3D; }

	void InitCameras() const;
	void AttachShaders(Shader* shader) const;
	void Update3DCamera() const;
	//void Use3DGBuffer() const;
	void Use2D() const;
	void Clear() const;
	void EnableUseOfTextures() const;
	void EnableCullFacing(GLenum mode) const;
	void DisableCullFacing() const;

	//void RegularLightingPass();
	//void GBufferPass();
	
};

