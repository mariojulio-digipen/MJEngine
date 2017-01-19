#pragma once

#include <glm.hpp>
#include "Quaternion.h"

typedef glm::vec3 Vector3;
typedef glm::mat4 Matrix4;

class VQM
{
private:

	Quaternion translator; // pure quaternion
	Quaternion rotator;
	Matrix4 scaler;

	VQM concatenate(VQM vqm_1, VQM vqm_2);
	Vector3 transformVector(Vector3 v, Vector3 translator, Quaternion rotator, Matrix4 scaler);

public:
	VQM();
	VQM(Vector3 V, Quaternion Q, Matrix4 S) :
		translator(Quaternion(0.0f, V)), rotator(Q), scaler(S)
	{
		rotator.Normalize();
	}
	~VQM();

	Vector3 operator*(Vector3 const & rhs);
	VQM operator*(VQM const & rhs);

	Vector3 GetV() { return translator.Vector; }
	Quaternion GetQ() { return rotator; }
	Matrix4 GetM() { return scaler; }

	Matrix4 VQM::ToMatrix();
};

