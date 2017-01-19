#include "VQM.h"


VQM::VQM()
{
	Quaternion I; // identity quaternion
	Matrix4 M; // identity matrix
	this->translator = Quaternion(0.0f, Vector3(0.0f)); // zero vector in pure quaternion form
	this->rotator = I;
	this->scaler = M;
}


VQM::~VQM()
{
}

VQM VQM::concatenate(VQM vqm_1, VQM vqm_2)
{
	//Using formula (2) from paper: 
	//[ 
	//	vqm_2*vqm2_1.GetV(), 
	//	vqm_2.GetQ * vqm_1.GetQ(), 
	//	(vqm_2.GetQ*vqm_2.GetM()*vqm_2.GetQ()^-1) * (vqm_1.GetQ*vqm_1.GetM()*vqm_1.GetQ()^-1)]
	// ]

	// compute the translation vector component
	Vector3 V = vqm_2 * vqm_1.GetV();

	// compute the quaternion component
	Quaternion Q = vqm_2.GetQ() * vqm_1.GetQ();

	// compute the matrix product
	Matrix4 ml = Quaternion::Qprod(vqm_2.GetQ(), Quaternion::Qprod(vqm_2.GetM(), vqm_2.GetQ() ^ -1));
	Matrix4 mr = Quaternion::Qprod(vqm_1.GetQ(), Quaternion::Qprod(vqm_1.GetM(), vqm_1.GetQ() ^ -1));
	Matrix4 M = ml * mr;

	return VQM(V, Q, M);
}


Vector3 VQM::operator*(Vector3 const & rhs)
{
	Vector3 v = this->translator.Vector;
	Quaternion q = this->rotator;
	Matrix4 m = this->scaler;
	return transformVector(rhs, v, q, m);
}

VQM VQM::operator*(VQM const & rhs)
{
	return concatenate(*this, rhs);
}

Vector3 VQM::transformVector(Vector3 v, Vector3 V, Quaternion Q, Matrix4 M)
{
	//Using formula (1) from paper: [Q*(M*r)*Q^-1] + V

	Quaternion r = Quaternion(0.0f, v);
	Quaternion rotM = M * r;
	Quaternion r_prime = Q * (rotM * (Q^-1));
	Quaternion pureV = Quaternion(0.0f, V);
	Vector3 result = (r_prime + pureV).Vector;
	return result;
}

Matrix4 VQM::ToMatrix()
{
	glm::mat4 Ms;
	Ms[0][0] = this->scaler[0][0];  //Ms[1][1] = Ms[2][2] =
	Ms[1][1] = this->scaler[1][1];
	Ms[2][2] = this->scaler[2][2];

	glm::mat4 Mv;
	Mv[3][0] = this->translator.Vector.x;
	Mv[3][1] = this->translator.Vector.y;
	Mv[3][2] = this->translator.Vector.z;

	glm::mat4 Mq = this->rotator.ToMatrix();
	Mq = glm::transpose(Mq);
	glm::mat4 M = Mv * Mq * Ms;

	return M;
}

