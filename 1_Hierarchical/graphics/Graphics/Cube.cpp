//#include "Cube.h"
//#include "FrameRateController.h"
//extern FrameRateController* frameRateController;
//
//Cube::Cube(bool activeColor, bool debugMode, Shader* defShad, Camera3D* mainCam)
//{
//	glObject = new GLObject(activeColor, debugMode);
//	this->defaultShader = defShad;
//	this->mainCamera = mainCam;
//}
//
//void Cube::CreateBuffers()
//{
//	glObject->Model = new ModelLoader;
//	glObject->Model->Name = "cube";
//	glObject->Model->ParseOBJ("Graphics\\ArtAssets\\exported\\sample.obj");
//	glObject->CreateBuffers();
//}
//
//void Cube::Init()
//{
//	glObject->Shader = this->defaultShader;
//	glObject->Shader->Disable2D();
//	glObject->Transform = new Transform;
//	glObject->Transform->Camera = this->mainCamera;
//	glObject->Init();
//	glObject->Shader->Enable2D();
//
//	glObject->Transform->Size = glm::vec3(1.0f, 2.0f, 1.0f);
//	glObject->Transform->RotationAngle.z = 45.0f;
//	/*glObject->Transform->Orientation.angle = 45.0f;
//	glObject->Transform->Orientation.axis = glm::vec3(0.0f, 0.0f, 1.0f);*/
//}
//
//void Cube::SendDataToGPU()
//{
//	glObject->UseVAO();
//	glObject->BuildVBOsFromOBJ(positionAttributeIndex, VERTEX);
//	glObject->BuildVBOsFromOBJ(uvAttributeIndex, UV);
//	glObject->BuildVBOsFromOBJ(normalAttributeIndex, NORMAL);
//	glObject->SizeOfData = glObject->Model->GetVertexData()->numberOfData / 3;
//	delete glObject->Model;
//}
//
//void Cube::Update()
//{
//	glObject->Transform->RotationAngle.y = glObject->Transform->RotationAngle.y + 1/* * frameRateController->GetFrameTime() * 0.1*/;
//	if (glObject->Transform->RotationAngle.y >= 360.0f)
//		glObject->Transform->RotationAngle.y = 0.0f;
//	/*glObject->Transform->Orientation.angle++;
//	if (glObject->Transform->Orientation.angle >= 360.0f)
//		glObject->Transform->Orientation.angle = 0.0f;*/
//	glObject->Transform->Position.x = -8.0f;
//	glObject->Transform->Position.y = 1.0f;
//	glObject->Shader->SetTransparency(0);
//	glObject->Update();
//}
//
