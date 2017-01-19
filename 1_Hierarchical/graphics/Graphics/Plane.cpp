//#include "Plane.h"
//
//Plane::Plane(bool activeColor, bool debugMode, Shader* defShad, Camera3D* mainCam)
//{
//	glObject = new GLObject(activeColor, debugMode);
//	this->defaultShader = defShad;
//	this->mainCamera = mainCam;
//}
//
//void Plane::CreateBuffers()
//{
//	glObject->Model = new ModelLoader;
//	glObject->Model->Name = "plane";
//	glObject->Model->ParseOBJ("Graphics\\ArtAssets\\exported\\samplePlane.obj");
//	glObject->CreateBuffers();
//}
//
//void Plane::Init()
//{
//	glObject->Shader = this->defaultShader;
//	glObject->Shader->Disable2D();
//	glObject->Transform = new Transform;
//	glObject->Transform->Camera = this->mainCamera;
//	glObject->Init();
//	glObject->Shader->Enable2D();
//}
//
//void Plane::SendDataToGPU()
//{
//	glObject->UseVAO();
//	glObject->BuildVBOsFromOBJ(positionAttributeIndex, VERTEX);
//	glObject->BuildVBOsFromOBJ(uvAttributeIndex, UV);
//	glObject->BuildVBOsFromOBJ(normalAttributeIndex, NORMAL);
//	glObject->SizeOfData = glObject->Model->GetVertexData()->numberOfData / 3;
//	delete glObject->Model;
//}
//
//void Plane::Update()
//{
//	glObject->Transform->Position.x = 0.0f;
//	glObject->Transform->Position.y = 0.0f;
//	glObject->Transform->Position.z = 0.0f;
//	glObject->Transform->Size.x = 20.0f;
//	glObject->Transform->Size.y = 0.05f;
//	glObject->Transform->Size.z = 20.0f;
//	glObject->Shader->SetTransparency(0);
//	glObject->Update();
//}
//
