//#include "Teapot.h"
//#include "FrameRateController.h"
//
//extern FrameRateController* frameRateController;
//
//Teapot::Teapot(bool activeColor, bool debugMode, Shader* defShad, Camera3D* mainCam)
//{
//	glObject = new GLObject(activeColor, debugMode);
//	this->defaultShader = defShad;
//	this->mainCamera = mainCam;
//}
//
//void Teapot::CreateBuffers()
//{
//	glObject->Model = new ModelLoader;
//	glObject->Model->Name = "teapot";
//	glObject->Model->ParseOBJ("Graphics\\ArtAssets\\exported\\teapot.obj");
//	glObject->CreateBuffers();
//}
//
//void Teapot::Init()
//{
//	glObject->Shader = this->defaultShader;
//	glObject->Shader->Disable2D();
//	glObject->Transform = new Transform;
//	glObject->Transform->Camera = this->mainCamera;
//	glObject->Init();
//	glObject->Shader->Enable2D();
//}
//
//void Teapot::SendDataToGPU()
//{
//	glObject->UseVAO();
//	glObject->BuildVBOsFromOBJ(positionAttributeIndex, VERTEX);
//	glObject->BuildVBOsFromOBJ(uvAttributeIndex, UV);
//	glObject->BuildVBOsFromOBJ(normalAttributeIndex, NORMAL);
//	glObject->SizeOfData = glObject->Model->GetVertexData()->numberOfData / 3;
//	delete glObject->Model;
//}
//
//void Teapot::Update()
//{
//	glObject->Transform->Position.x = 4.0f;
//	glObject->Transform->Position.y = 1.0f;
//
//	glObject->Transform->RotationAngle.x = 0.0f;
//	glObject->Transform->RotationAngle.y = glObject->Transform->RotationAngle.y - 1.0f * frameRateController->GetFrameTime() * 0.1f;
//	if (glObject->Transform->RotationAngle.y <= -360.0f)
//		glObject->Transform->RotationAngle.y = 0.0f;
//	glObject->Transform->RotationAngle.z = 0.0f;
//	/*glObject->Transform->Orientation.angle--;
//	if (glObject->Transform->Orientation.angle <= -360.0f)
//		glObject->Transform->Orientation.angle = 0.0f;*/
//	glObject->Transform->Size = glm::vec3(1.0f, 1.0f, 1.0f);
//	glObject->Shader->SetTransparency(0);
//	glObject->Update();
//}
//
