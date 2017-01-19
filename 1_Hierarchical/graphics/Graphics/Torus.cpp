//#include "Torus.h"
//
//Torus::Torus(bool activeColor, bool debugMode, Shader* defShad, Camera3D* mainCam)
//{
//	glObject = new GLObject(activeColor, debugMode);
//	this->defaultShader = defShad;
//	this->mainCamera = mainCam;
//}
//
//void Torus::CreateBuffers()
//{
//	glObject->Model = new ModelLoader;
//	glObject->Model->Name = "torus";
//	glObject->Model->ParseOBJ("Graphics\\ArtAssets\\exported\\sampleTorus.obj");
//	glObject->CreateBuffers();
//}
//
//void Torus::Init()
//{
//	glObject->Shader = this->defaultShader;
//	glObject->Shader->Disable2D();
//	glObject->Transform = new Transform;
//	glObject->Transform->Camera = this->mainCamera;
//	glObject->Init();
//	glObject->Shader->Enable2D();
//}
//
//void Torus::SendDataToGPU()
//{
//	glObject->UseVAO();
//	glObject->BuildVBOsFromOBJ(positionAttributeIndex, VERTEX);
//	glObject->BuildVBOsFromOBJ(uvAttributeIndex, UV);
//	glObject->BuildVBOsFromOBJ(normalAttributeIndex, NORMAL);
//	glObject->SizeOfData = glObject->Model->GetVertexData()->numberOfData / 3;
//	delete glObject->Model;
//}
//
//void Torus::Update()
//{
//	glObject->Transform->RotationAngle.y++;
//	if (glObject->Transform->RotationAngle.y >= 360.0f)
//		glObject->Transform->RotationAngle.y = 0.0f;
//
//	/*glObject->Transform->Orientation.angle++;
//	if (glObject->Transform->Orientation.angle >= 360.0f)
//		glObject->Transform->Orientation.angle = 0.0f;*/
//	glObject->Transform->Position.y = 5.0f;
//	glObject->Transform->Position.z = -15.0f;
//	glObject->Transform->Size.x = 0.5f;
//	glObject->Transform->Size.y = 2.0f;
//	glObject->Transform->Size.z = 1.0f;
//	glObject->Update();
//}
//
