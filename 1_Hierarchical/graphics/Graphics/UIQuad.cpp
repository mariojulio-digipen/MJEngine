//#include "UIQuad.h"
//#include "Shader.h"
//#include "InputManager.h"
//#include "Camera2D.h"
//
//#include <gtc\matrix_transform.hpp>
//#include <gtx\transform.hpp>
//
//UIQuad::UIQuad(bool activeColor, bool debugMode, Shader* defShad, Camera2D* mainCam)
//{
//	glObject = new GLObject(activeColor, debugMode);
//	this->defaultShader = defShad;
//	this->mainCamera = mainCam;
//}
//
//void UIQuad::CreateBuffers()
//{
//	glObject->Model = new ModelLoader;
//	glObject->Model->Name = "2DQuad";
//	glObject->Model->ParseOBJ("Graphics\\ArtAssets\\exported\\UIQuad.obj");
//	glObject->CreateBuffers();
//}
//
//void UIQuad::Init()
//{
//	glObject-
//= this->defaultShader;
//	glObject->Shader->Enable2D();
//	glObject->Transform = new Transform;
//	glObject->Transform->Camera2D = this->mainCamera;
//	glObject->Init();
//	glObject->Shader->Disable2D();
//}
//
//void UIQuad::SendDataToGPU()
//{
//	glObject->UseVAO();
//	glObject->BuildVBOsFromOBJ(positionAttributeIndex, VERTEX);
//	glObject->BuildVBOsFromOBJ(uvAttributeIndex, UV);
//	glObject->SizeOfData = glObject->Model->GetVertexData()->numberOfData / 3;
//	delete glObject->Model;
//}
//
//void UIQuad::Update()
//{
//	/*glm::vec3 Q = this->mainCamera->Position - glObject->Transform->Position;
//	glm::vec3 C = this->mainCamera->Position;
//	float angle = glm::acos(glm::dot(camViewDirection, posToQuadVec));
//	glm::mat4 rot = glm::rotate(-this->mainCamera->moveDelta.x, this->mainCamera->Up) *
//		glm::rotate(-this->mainCamera->moveDelta.y, this->mainCamera->Right);
//	Q = glm::mat3(rot) * Q;
//	glObject->Transform->Position = C - Q;
//
//
//	glm::vec3 quadDirection = this->mainCamera->ViewDirection;
//	float angleX, angleY, dot;
//
//	glm::vec3 baseVector = glm::vec3(0.0f, 0.0f, -1.0f);
//	dot = glm::dot(normalize(baseVector), normalize(quadDirection));
//	angleX = quadDirection.y < 0 ? -acos(dot) : acos(dot);
//
//	glObject->Transform->RotationAngle.x = glObject->Transform->RotationAngle.x + glm::degrees(-this->mainCamera->moveDelta.y);
//	glObject->Transform->RotationAngle.y = glObject->Transform->RotationAngle.y + glm::degrees(-this->mainCamera->moveDelta.x);
//	printf("angle(%f, %f, %f)\n", glObject->Transform->RotationAngle.x, glObject->Transform->RotationAngle.y, glObject->Transform->RotationAngle.z);
//	if (glObject->Transform->RotationAngle.y > 0.1f || glObject->Transform->RotationAngle.y < -0.1f)
//	glObject->Transform->RotationAngle.x = glObject->Transform->RotationAngle.x - glm::degrees(-this->mainCamera->moveDelta.y);
//	glm::vec3 baseVector2 = glm::vec3(0.0f, 1.0f, 0.0f);
//	dot = glm::dot(normalize(baseVector2), normalize(quadDirection));
//	angleY = quadDirection.x < 0 ? -acos(dot) : acos(dot);
//	glObject->Transform->RotationAngle.y = glm::degrees(angleY) - 90.0f;
//
//
//
//	baseVector = glm::vec3(0.0f, 1.0f, 0.0f);
//	dot = glm::dot(normalize(baseVector), normalize(quadDirection));
//	angleY = quadDirection.x < 0 ? acos(dot) : -acos(dot);
//	glObject->Transform->RotationAngle.y = 90.0f - glm::degrees(angleY);
//*/
//	
//	glObject->Shader->SetLightsOff();
//	float x = -mainCamera->WinWidth*0.5f + glObject->Transform->Size.x;
//	float y = mainCamera->WinHeight*0.5f - glObject->Transform->Size.y;
//	glObject->Transform->Position = glm::vec3(x, y, 0.0f);
//	glObject->Update();
//	glObject->Shader->SetLightsOn();
//}
