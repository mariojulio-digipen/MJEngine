#include "Component.h"

#include <iostream>
#include <fstream>
#include <sstream>



Component::Component()
{

}


Component::~Component()
{
}

std::string Component::readGameObjectFile(const char* fileName)
{
	std::string line;
	std::stringstream fileString;
	std::ifstream file(fileName);
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			fileString << line << "\n";
		}
		file.close();
	}
	else
	{
		printf("Unable to open model file.");
	}
	return fileString.str();
}