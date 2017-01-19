#pragma once

#include "Component.h"
#include <SDL_stdinc.h>
#include <vector>



class TransformComponent;
class PainterComponent;

struct ControllerText
{
	GameObject* label;
	std::string labelName;
	TransformComponent* labelTrans;
	float posX, posY, posZ;
	float rotX, rotY, rotZ;
	float sizeX, sizeY, sizeZ;
	std::string text;
	int r, g, b, wrapLength;
};

struct ControllerImage
{
	GameObject* imgObj;
	float posX, posY, posZ;
	float rotX, rotY, rotZ;
	float sizeX, sizeY, sizeZ;
	TransformComponent* imgTrans;
};

struct ControllerElements
{
	ControllerText textName;
	ControllerImage uix, uiy, uiz,
		uiMoreX, uiLessX,
		uiMoreY, uiLessY,
		uiMoreZ, uiLessZ;
};


class UILightController : public Component
{
private:
	TransformComponent* transform;
	PainterComponent* painter;

	/*ControllerText title, labelPosition, labelColor;
	ControllerImage uix, uiy, uiz, 
		uiMoreX, uiLessX, 
		uiMoreY, uiLessY, 
		uiMoreZ, uiLessZ;*/
	ControllerText title;
	ControllerElements positionControl, colorControl;

	void createControl(ControllerElements* control);


	void createLabel(ControllerText* labelBox);
	void createImage(ControllerImage* img);
	void serializeLabel(std::istringstream* objcontentStream);
	void serializeLabelProps(std::istringstream* objcontentStream, ControllerElements* controllerElement);

	void showThisObject(bool show);
	void addElementsTransProps(ControllerImage* controller, std::istringstream* objcontentStream);
	void addElementsTransProps(ControllerText* controller, std::istringstream* objcontentStream);
	void clickOnImage();



	// this function must be part of the shape collision manager
	bool mouseIsOverElement();

	std::vector<GameObject*> children;

	//template<class T>
	//void serializePropsToElements(T* controller, std::istringstream* objcontentStream);

public:
	UILightController() {}
	~UILightController() {}

	bool Init();
	void Update();
	void Destroy();
	void CompleteExtraSetup();
	void Serialize(std::istringstream* objcontentStream);
};

