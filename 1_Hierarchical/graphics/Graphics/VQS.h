#pragma once

#include <glm.hpp>
#include "Quaternion.h"

class VQS
{
private:
	
	VQS concatenate(VQS vqs2, VQS vqs_1);

	// helper functions
	glm::vec3 getTransform(glm::vec3 v, glm::vec3 translator, Quaternion rotator, float scaler);


public:

	VQS();
	VQS(glm::vec3 V, Quaternion Q, float S) :
		Translator(Quaternion(0.0f, V)), Rotator(Q), ScaleUniform(S) 
	{
		Rotator.Normalize();
	}
	~VQS();

	// Pure quaternion that contains the translation vector
	// Stored this way for programming convience, because in
	// some functions, I just use this translator quaternion
	// to compute final result and then extract 
	// the vector and return it
	Quaternion Translator; 

	// This is the rotation quaternion
	Quaternion Rotator;

	// This is the uniform scale
	float ScaleUniform;


	
	glm::vec3 operator*(glm::vec3 const & rhs);
	VQS operator*(VQS const & rhs);

	glm::mat4 ToMatrix(bool local);
	glm::vec3 GetUniformScale();

	static VQS Interpolate(VQS from_vqs0, VQS to_vqs1, float u);
	static VQS TranslateLerp(VQS from_vqs0, VQS to_vqs1, float u);
};

