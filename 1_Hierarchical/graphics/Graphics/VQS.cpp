#include "VQS.h"



VQS::VQS()
{
	Quaternion I;
	this->Translator = Quaternion(0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	this->Rotator = I;
	this->ScaleUniform = 1.0f;
}


VQS::~VQS()
{
}

glm::vec3 VQS::getTransform(glm::vec3 r,
	glm::vec3 translator, Quaternion rotator, float scaler)
{
	Quaternion pure_r = Quaternion(0.0f, glm::vec3(r.x, r.y, r.z));
	Quaternion rotated = rotator*(pure_r*(rotator^-1));

	Quaternion rotator_inverse = rotator^-1;

	Quaternion scaled_rotated = scaler * rotated;
	Quaternion pure_trans = Quaternion(0.0f, translator);
	Quaternion translated_scaled_rotated = scaled_rotated + pure_trans;
	glm::vec3 result = translated_scaled_rotated.Vector;
	return result;
}

glm::vec3 VQS::operator*(glm::vec3 const & rhs)
{
	glm::vec3 translator = this->Translator.Vector;
	Quaternion rotator = this->Rotator;
	float scaler = this->ScaleUniform;
	glm::vec3 res = getTransform(rhs, translator, rotator, scaler);
	return res;
}

glm::mat4 VQS::ToMatrix(bool local)
{
	glm::mat4 Ms;
	Ms[0][0] = Ms[1][1] = Ms[2][2] = this->ScaleUniform;

	glm::mat4 Mv;
	Mv[3][0] = this->Translator.Vector.x;
	Mv[3][1] = this->Translator.Vector.y;
	Mv[3][2] = this->Translator.Vector.z;
	//Mv = glm::transpose(Mv);

	glm::mat4 Mq = this->Rotator.ToMatrix();
	Mq = glm::transpose(Mq);
	glm::mat4 M;
	if(local)
		M = Mv * Mq * Ms;
	else
		M = Mq * Mv * Ms;
	//M = glm::transpose(M);
	//M = Ms * Mq * Mv;
	return M;
}


VQS VQS::concatenate(VQS vqs_2, VQS vqs_1)
{	
	VQS t2 = vqs_2;
	glm::vec3 V1 = vqs_1.Translator.Vector;


	glm::vec3 V = t2 * V1;
	Quaternion Q = vqs_2.Rotator * vqs_1.Rotator;
	float S = vqs_2.ScaleUniform * vqs_1.ScaleUniform;

	return VQS(V, Q, S);

}

VQS VQS::operator*(VQS const & rhs)
{
	return concatenate((*this), rhs);
}

glm::vec3 VQS::GetUniformScale()
{
	return glm::vec3(this->ScaleUniform, this->ScaleUniform, this->ScaleUniform);
}

//
VQS VQS::Interpolate(VQS from_vqs0, VQS to_vqs1, float u)
{
	// 1. Interpolate the translator as a vec3
	glm::vec3 from_v0 = from_vqs0.Translator.Vector;
	glm::vec3 to_v1 = to_vqs1.Translator.Vector;
	glm::vec3 newvec = (1 - u)*from_v0 + u*to_v1;

	// 2. Interpolate the rotator (quaternion slerp)
	Quaternion from_q0 = from_vqs0.Rotator;
	Quaternion to_q1 = to_vqs1.Rotator;
	Quaternion newquat = Quaternion::Slerp(from_q0, to_q1, u);

	// 3. Interpolate the scaler
	float from_s0 = from_vqs0.ScaleUniform;
	float to_s1 = to_vqs1.ScaleUniform;
	float newscaler = from_s0*glm::pow((to_s1 / from_s0), u);

	return VQS(newvec, newquat, newscaler);
}

VQS VQS::TranslateLerp(VQS from_vqs0, VQS to_vqs1, float u)
{
	glm::vec3 from_v0 = from_vqs0.Translator.Vector;
	glm::vec3 to_v1 = to_vqs1.Translator.Vector;
	glm::vec3 newvec = (1 - u)*from_v0 + u*to_v1;

	return  VQS(newvec, from_vqs0.Rotator, from_vqs0.ScaleUniform);
}