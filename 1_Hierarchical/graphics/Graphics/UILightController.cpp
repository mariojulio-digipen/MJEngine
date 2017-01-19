#include "UILightController.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "TransformComponent.h"
#include "InputManager.h"
#include "MouseManager.h"
#include "PainterComponent.h"
#include "Font.h"
#include <SDL_ttf.h>
#include <algorithm>
#include <functional>


extern InputManager* inputManager;
extern MouseManager* mouseManager;
extern ResourceManager* resourceManager;

bool UILightController::Init()
{

	transform = GetOwner()->FindComponent<TransformComponent>();
	painter = GetOwner()->FindComponent<PainterComponent>();
	return true;
}

void UILightController::showThisObject(bool show)
{
	painter->SetVisible(show);
	std::vector<GameObject*>::iterator childit = children.begin();
	while (childit != children.end())
	{
		(*childit)->FindComponent<PainterComponent>()->SetVisible(show);
		childit = childit + 1;
	}
}

void UILightController::clickOnImage()
{
	if (painter->IsVisible())
	{
		//TODO
	}
}

void UILightController::Update()
{
	/*title.label->GetGLObject()->Texture->GetFont()->Free();
	title.label->GetGLObject()->Texture->CreateFromText(title.text,
		{ static_cast<Uint8>(title.r), static_cast<Uint8>(title.g), static_cast<Uint8>(title.g) });*/
	
	/*labelPosition.label->GetGLObject()->Texture->GetFont()->Free();
	labelPosition.label->GetGLObject()->Texture->CreateFromText(labelPosition.text,
		{ static_cast<Uint8>(labelPosition.r), static_cast<Uint8>(labelPosition.g), static_cast<Uint8>(labelPosition.g) });*/
	
	if (inputManager->IsKeyPressed(SDL_SCANCODE_1))
		showThisObject(false);

	if (inputManager->IsKeyPressed(SDL_SCANCODE_2))
		showThisObject(true);

}

void UILightController::Destroy()
{
	std::vector<Texture*> textures = title.label->GetGLObject()->Textures;
	std::vector<Texture*>::iterator it = textures.begin();
	while (it != textures.end())
	{
		if (!*it)
			printf("Error: Trying to destroy a NULL texture");
		(*it)->Free();
		it = it + 1;
	}
	delete title.label;

}

void UILightController::CompleteExtraSetup()
{
	title.label = resourceManager->CreateOneGameObject("TextCanvas");
	//resourceManager->AttachGameObjectToSystem(title.label);
	createLabel(&title);

	createControl(&positionControl);
	createControl(&colorControl);
	



	//painter->SetVisible(false); //TODO

}

void UILightController::Serialize(std::istringstream* objcontentStream)
{
	serializeLabel(objcontentStream);
}

void UILightController::createImage(ControllerImage* img)
{
	children.push_back(img->imgObj);

	img->imgTrans = img->imgObj->FindComponent<TransformComponent>();

	glm::vec3 actualPos;
	actualPos.x = img->posX;
	actualPos.y = img->posY;

	img->imgTrans->SetPosition(transform->GetPosition() + actualPos);
	img->imgTrans->SetRotation(transform->GetRotation() + glm::vec3(img->rotX, img->rotY, img->rotZ));
	img->imgTrans->SetScale(glm::vec3(img->sizeX, img->sizeY, img->sizeZ));
}

void UILightController::createLabel(ControllerText* labelBox)
{
	children.push_back(labelBox->label);
	labelBox->labelTrans = labelBox->label->FindComponent<TransformComponent>();


	glm::vec3 actualPos;
	actualPos.x = labelBox->posX;
	actualPos.y = labelBox->posY;

	labelBox->labelTrans->SetPosition(transform->GetPosition() + actualPos);
	labelBox->labelTrans->SetRotation(transform->GetRotation() + glm::vec3(labelBox->rotX, labelBox->rotY, labelBox->rotZ));
	labelBox->labelTrans->SetScale(glm::vec3(labelBox->sizeX, labelBox->sizeY, labelBox->sizeZ));

	//Font* font = labelBox->label->GetGLObject()->Textures[0]->GetFont();
	GameObject* tmpobj = labelBox->label;
	GLObject* tmpglobj = tmpobj->GetGLObject();
	Texture* tmptxt = tmpglobj->Textures[0];
	Font* font = tmptxt->GetFont();
	
	font->Free();
	labelBox->label->GetGLObject()->Textures[0]->CreateFromText(labelBox->text,
		{ static_cast<Uint8>(labelBox->r), static_cast<Uint8>(labelBox->g), static_cast<Uint8>(labelBox->g) });


}

void UILightController::serializeLabel(std::istringstream* objcontentStream)
{
	std::string line;
	std::getline((*objcontentStream), line);

	std::istringstream lineStream(line.c_str());
	std::string word;
	while ((*objcontentStream) >> word)
	{

		if (word == "elementControl")
		{
			std::getline((*objcontentStream), line);
			line.erase(
				std::remove_if(line.begin(),
					line.end(), isspace),
				line.end()
			);

			if (line == "TitleControl")
			{

				while ((*objcontentStream) >> word)
				{
					if (word == "element")
					{
						std::getline((*objcontentStream), line);
						line.erase(
							std::remove_if(line.begin(),
								line.end(), isspace),
							line.end()
						);

						if (line == "title")
						{
							title.labelName = line;
							addElementsTransProps(&title, objcontentStream);
							continue;
						}

						if (word == "endTitleControl")
						{
							std::getline((*objcontentStream), line);
							break;
						}
					}
					break;
				}
				continue;
			
			}

			if (line == "PositionControl")
			{
				serializeLabelProps(objcontentStream, &positionControl);
				continue;
			}

			if (line == "ColorControl")
			{
				serializeLabelProps(objcontentStream, &colorControl);
				continue;
			}
		}
	}
}

void UILightController::serializeLabelProps(std::istringstream* objcontentStream, ControllerElements* controllerElement)
{
	std::string line;
	std::string word;
	while ((*objcontentStream) >> word)
	{
		if (word == "element")
		{
			std::getline((*objcontentStream), line);
			line.erase(
				std::remove_if(line.begin(),
					line.end(), isspace),
				line.end()
			);

			if (line == "labelText")
			{
				controllerElement->textName.labelName = line;
				addElementsTransProps(&controllerElement->textName, objcontentStream);
				continue;
			}

			if (line == "uix")
			{
				addElementsTransProps(&controllerElement->uix, objcontentStream);
				continue;
			}

			if (line == "morex")
			{
				addElementsTransProps(&controllerElement->uiMoreX, objcontentStream);
				continue;
			}

			if (line == "lessx")
			{
				addElementsTransProps(&controllerElement->uiLessX, objcontentStream);
				continue;
			}

			if (line == "uiy")
			{
				addElementsTransProps(&controllerElement->uiy, objcontentStream);
				continue;
			}

			if (line == "morey")
			{
				addElementsTransProps(&controllerElement->uiMoreY, objcontentStream);
				continue;
			}

			if (line == "lessy")
			{
				addElementsTransProps(&controllerElement->uiLessY, objcontentStream);
				continue;
			}

			if (line == "uiz")
			{
				addElementsTransProps(&controllerElement->uiz, objcontentStream);
				continue;
			}

			if (line == "morez")
			{
				addElementsTransProps(&controllerElement->uiMoreZ, objcontentStream);
				continue;
			}

			if (line == "lessz")
			{
				addElementsTransProps(&controllerElement->uiLessZ, objcontentStream);
				continue;
			}

			if (word == "endControl")
			{
				std::getline((*objcontentStream), line);
				break;
			}
		}
		break;
	}
}

void UILightController::addElementsTransProps(ControllerImage* controller, std::istringstream* objcontentStream)
{
	std::string line;
	std::string word;
	while ((*objcontentStream) >> word)
	{
		if (word == "#")
		{
			std::getline((*objcontentStream), line);
			continue;
		}
		if (word == "pos")
		{
			std::getline((*objcontentStream), line);
			sscanf_s(line.c_str(), "%f %f %f", &controller->posX, &controller->posY, &controller->posZ);
			continue;
		}
		if (word == "rot")
		{
			std::getline((*objcontentStream), line);
			sscanf_s(line.c_str(), "%f %f %f", &controller->rotX, &controller->rotY, &controller->rotZ);
			continue;
		}
		if (word == "size")
		{
			std::getline((*objcontentStream), line);
			sscanf_s(line.c_str(), "%f %f %f", &controller->sizeX, &controller->sizeY, &controller->sizeZ);
			continue;
		}
		if (word == "endelement")
		{
			std::getline((*objcontentStream), line);
			break;
		}
	}
}

void UILightController::addElementsTransProps(ControllerText* controller, std::istringstream* objcontentStream)
{
	std::string line;
	std::string word;
	while ((*objcontentStream) >> word)
	{
		if (word == "#")
		{
			std::getline((*objcontentStream), line);
			continue;
		}

		if (word == "text")
		{
			std::getline((*objcontentStream), line);
			controller->text = line;
			continue;
		}
		if (word == "pos")
		{
			std::getline((*objcontentStream), line);
			sscanf_s(line.c_str(), "%f %f %f", &controller->posX, &controller->posY, &controller->posZ);
			continue;
		}
		if (word == "rot")
		{
			std::getline((*objcontentStream), line);
			sscanf_s(line.c_str(), "%f %f %f", &controller->rotX, &controller->rotY, &controller->rotZ);
			continue;
		}
		if (word == "size")
		{
			std::getline((*objcontentStream), line);
			sscanf_s(line.c_str(), "%f %f %f", &controller->sizeX, &controller->sizeY, &controller->sizeZ);
			continue;
		}
		if (word == "color")
		{
			std::getline((*objcontentStream), line);
			sscanf_s(line.c_str(), "%i %i %i", &controller->r, &controller->g, &controller->b);
			continue;
		}
		if (word == "wrap")
		{
			std::getline((*objcontentStream), line);
			controller->wrapLength = std::stoi(line);
			continue;
		}
		if (word == "endelement")
		{
			std::getline((*objcontentStream), line);
			break;
		}
	}
}

void UILightController::createControl(ControllerElements* control)
{
	control->textName.label = resourceManager->CreateOneGameObject("TextCanvas");
	//resourceManager->AttachGameObjectToSystem(control->textName.label);
	createLabel(&control->textName);

	control->uix.imgObj = resourceManager->CreateOneGameObject("UI_X");
	//resourceManager->AttachGameObjectToSystem(control->uix.imgObj);
	createImage(&control->uix);

	control->uiMoreX.imgObj = resourceManager->CreateOneGameObject("UI_More");
	//resourceManager->AttachGameObjectToSystem(control->uiMoreX.imgObj);
	createImage(&control->uiMoreX);

	control->uiLessX.imgObj = resourceManager->CreateOneGameObject("UI_Less");
	//resourceManager->AttachGameObjectToSystem(control->uiLessX.imgObj);
	createImage(&control->uiLessX);

	control->uiy.imgObj = resourceManager->CreateOneGameObject("UI_Y");
	//resourceManager->AttachGameObjectToSystem(control->uiy.imgObj);
	createImage(&control->uiy);

	control->uiMoreY.imgObj = resourceManager->CreateOneGameObject("UI_More");
	//resourceManager->AttachGameObjectToSystem(control->uiMoreY.imgObj);
	createImage(&control->uiMoreY);

	control->uiLessY.imgObj = resourceManager->CreateOneGameObject("UI_Less");
	//resourceManager->AttachGameObjectToSystem(control->uiLessY.imgObj);
	createImage(&control->uiLessY);

	control->uiz.imgObj = resourceManager->CreateOneGameObject("UI_Z");
	//resourceManager->AttachGameObjectToSystem(control->uiz.imgObj);
	createImage(&control->uiz);

	control->uiMoreZ.imgObj = resourceManager->CreateOneGameObject("UI_More");
	//resourceManager->AttachGameObjectToSystem(control->uiMoreZ.imgObj);
	createImage(&control->uiMoreZ);

	control->uiLessZ.imgObj = resourceManager->CreateOneGameObject("UI_Less");
	//resourceManager->AttachGameObjectToSystem(control->uiLessZ.imgObj);
	createImage(&control->uiLessZ);
}

bool UILightController::mouseIsOverElement()
{

	return false;
}