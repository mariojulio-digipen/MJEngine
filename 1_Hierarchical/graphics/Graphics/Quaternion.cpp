#include "Quaternion.h"
#include <exception>

// This operation is commutative. So, q1 + q2 == q2 + q1
Quaternion Quaternion::operator+(const Quaternion& rhs)
{
	Quaternion q1 = *this;
	Quaternion q2 = rhs;
	return addition(q1, q2);
}

// This operation is commutative. So, q1 + q2 == q2 + q1
Quaternion Quaternion::addition(Quaternion q1, Quaternion q2)
{
	float S1 = q1.Scalar;
	Vector3 V1 = q1.Vector;

	float S2 = q2.Scalar;
	Vector3 V2 = q2.Vector;

	float newScalar = S1 + S2;
	Vector3 newVector = V1 + V2;
	
	return Quaternion(newScalar, newVector);
}

Quaternion operator*(Quaternion const & q, float const & C)
{
	Quaternion qx;
	qx.scalarMultiplication(C, q);
	return qx;
}

Quaternion operator*(float const & C, Quaternion const & q)
{
	Quaternion qx;
	return qx.scalarMultiplication(C, q);
}



Quaternion Quaternion::scalarMultiplication(float s, Quaternion q)
{
	float S = q.Scalar;
	Vector3 V = q.Vector;

	float newScalar = s * S;
	Vector3 newVector = s * V;

	return Quaternion(newScalar, newVector);
}

// Important: Result is always q1 * q2. If you want q2 * q1, just
// provide q2 as first term, and q1 as the second one.
Quaternion Quaternion::operator*(Quaternion const & rhs)
{
	return multiplication(*this, rhs);
}

//Matrix4 Quaternion::operator*(Matrix4 const & HMatrix)
//{
//	Quaternion qx;
//	return qx.Qprod(*this, HMatrix);
//	//return Matrix4();
//}

// product of a vec3 times a quaternion
Quaternion operator*(Vector3 lhs, Quaternion rhs)
{
	Quaternion pure_quat(0.0f, lhs);
	Quaternion tmp = rhs * (pure_quat * rhs^-1);
	return tmp;
}

Quaternion operator*(Matrix4 lhs, Quaternion rhs)
{
	Quaternion qx;
	return qx.multiplication(lhs, rhs);
}

// Important: Result is always q1 * q2. If you want q2 * q1, just
// provide q2 as first parameter, and q1 as the second one.
Quaternion Quaternion::multiplication(Quaternion q1, Quaternion q2)
{
	float S1 = q1.Scalar;
	Vector3 V1 = q1.Vector;
	float S2 = q2.Scalar;
	Vector3 V2 = q2.Vector;

	float newScalar = S1*S2 - glm::dot(V1, V2);
	Vector3 newVector = S1*V2 + S2*V1 + glm::cross(V1, V2);

	return Quaternion(newScalar, newVector);
}

Quaternion Quaternion::multiplication(Matrix4 hmatrix, Quaternion q)
{
	float scalar =	hmatrix[0][3] * q.Scalar +
		hmatrix[1][3] * q.Scalar + 
		hmatrix[2][3] * q.Scalar + 
		hmatrix[3][3] * q.Scalar;

	float x = hmatrix[0][0] * q.Vector.x +
		hmatrix[1][0] * q.Vector.x +
		hmatrix[2][0] * q.Vector.x +
		hmatrix[3][0] * q.Vector.x;

	float y = hmatrix[0][1] * q.Vector.y +
		hmatrix[1][1] * q.Vector.y +
		hmatrix[2][1] * q.Vector.y +
		hmatrix[3][1] * q.Vector.y;

	float z = hmatrix[0][2] * q.Vector.z +
		hmatrix[1][2] * q.Vector.z +
		hmatrix[2][2] * q.Vector.z +
		hmatrix[3][2] * q.Vector.z;

	return Quaternion(scalar, Vector3(x, y, z));
}

float Quaternion::Dot(Quaternion q1, Quaternion q2)
{
	float S1 = q1.Scalar;
	Vector3 V1 = q1.Vector;
	float S2 = q2.Scalar;
	Vector3 V2 = q2.Vector;

	float s1 = S1*S2;
	float s2 = glm::dot(V1, V2);

	float newScalar = s1 + s2;
	return newScalar;
}


float Quaternion::Magnitude(Quaternion q)
{
	/*
		Since the projection of a vector on to itself leaves 
		its magnitude unchanged, the dot product of any vector 
		with itself is the square of that vector's magnitude.

		A · A = AA cos 0° = A^2
	*/
	float S = q.Scalar;
	Vector3 V = q.Vector;

	float VSquared = V.x*V.x + V.y*V.y + V.z*V.z;
	float res = glm::sqrt(S*S + VSquared);
	return res;
}

Quaternion Quaternion::operator-()
{
	return this->conjugate();
}

Quaternion Quaternion::conjugate()
{
	Vector3 V = -this->Vector;
	return Quaternion(this->Scalar, V);
}

// power must be -1
Quaternion Quaternion::operator^(int const & s)
{
	if (s != -1)
		throw std::exception("Provide -1 for inverse. Power >=0 is not defined for quaternions.");

	return inverse(*this);
}

Quaternion Quaternion::inverse(Quaternion q)
{
	Quaternion numerator = q.conjugate();

	float S = q.Scalar;
	Vector3 V = q.Vector;

	float denominator = S*S + V.x*V.x + V.y*V.y + V.z*V.z;

	return numerator / denominator;
}

Quaternion operator/(Quaternion const & lhs, float const & rhs)
{
	float S = lhs.Scalar / rhs;
	Vector3 V = lhs.Vector / rhs;

	return Quaternion(S, V);
}

Quaternion Quaternion::operator/(Quaternion const & rhs)
{
	return divide(*this, rhs);
}

// Important: q1 is numerator, q2 is denominator
Quaternion Quaternion::divide(Quaternion q1, Quaternion q2)
{
	Quaternion result = q1 * (q2^-1);
	return result;
}

// this function is only a test, please disregard
Vector3 Quaternion::RotateFast(float angle, Vector3 angle_axis,
	Vector3 vector_to_rotate)
{
	Vector3 A = glm::normalize(angle_axis);
	Vector3 r = vector_to_rotate;

	Vector3 rp = glm::cos(angle) * r + 
		(1 - glm::cos(angle))*(r - (glm::dot(A, r))) * A + 
		glm::sin(angle)*glm::cross(A, r);

	return rp;
}
// this function is only a test, please disregard
Vector3 Quaternion::RotateFast2(float angle, Vector3 angle_axis,
	Vector3 vector_to_rotate)
{
	Vector3 A = glm::normalize(angle_axis);
	Vector3 r = vector_to_rotate;
	Vector3 V = glm::sin(angle * 0.5f) * A;
	float S = glm::cos(angle * 0.5f);
	Vector3 rp = 
		S*S*r + 
		S*glm::cross(V,r) + glm::dot(V,r)*V +
		S*(glm::cross(V, r)) - glm::dot(V,V)*r + glm::dot(V, r)*V;

	/*Vector3 rp = glm::cos(angle) * r +
		(1 - glm::cos(angle))*(r - (glm::dot(A, r))) * A +
		glm::sin(angle)*glm::cross(A, r);*/

	return rp;
}

Vector3 Quaternion::Rotate(float angle, Vector3 angle_axis,
	Vector3 vector_to_rotate)
{
	Vector3 A = glm::normalize(angle_axis);
	Vector3 r = vector_to_rotate;

	float S = glm::cos(angle* 0.5f);
	Vector3 V = glm::sin(angle* 0.5f) * A;

	Quaternion q(S, V);
	Quaternion p(0.0f, r);

	Quaternion q_inv = q^-1;

	Quaternion Q = q*(p*q_inv);
	return Q.Vector;
}

Vector3 Quaternion::Rotate(Quaternion q, Vector3 v)
{
	Quaternion p(0.0f, v);
	Quaternion q_inv = q^-1;

	Quaternion Q = q*(p*q_inv);

	return Q.Vector;
}



Quaternion Quaternion::QuatFromAxisAngle(float angle, Vector3 axis)
{
	Vector3 A = glm::normalize(axis);

	float S = glm::cos(angle* 0.5f);
	Vector3 V = glm::sin(angle* 0.5f) * A;


	return Quaternion(S, V);
}

Quaternion Quaternion::QuatFromAngles(Vector3 angle)
{
	return QuatFromAxisAngle(angle.z, Vector3(0.0f, 0.0f, 1.0f)) *
		QuatFromAxisAngle(angle.y, Vector3(0.0f, 1.0f, 0.0f)) *
		QuatFromAxisAngle(angle.x, Vector3(1.0f, 0.0f, 0.0f));
}

Matrix4 Quaternion::ToMatrix()
{
	float x = this->Vector.x;
	float y = this->Vector.y;
	float z = this->Vector.z;
	float s = this->Scalar;

	float xx = x * x;
	float yy = y * y;
	float zz = z * z;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float sx = s * x;
	float sy = s * y;
	float sz = s * z;

	Matrix4 Mq;
	Mq[0][0] = 1.0f - 2.0f * (yy + zz);
	Mq[0][1] = 2.0f * (xy - sz);
	Mq[0][2] = 2.0f * (xz + sy);
	Mq[0][3] = 0.0f;

	Mq[1][0] = 2.0f * (xy + sz);
	Mq[1][1] = 1.0f - 2.0f * (xx + zz);
	Mq[1][2] = 2.0f * (yz - sx);
	Mq[1][3] = 0.0f;

	Mq[2][0] = 2.0f * (xz - sy);
	Mq[2][1] = 2.0f * (yz + sx);
	Mq[2][2] = 1.0f - 2.0f * (xx + yy);
	Mq[2][3] = 0.0f;

	Mq[3][0] = 0.0f;
	Mq[3][1] = 0.0f;
	Mq[3][2] = 0.0f;
	Mq[3][3] = 1.0f;

	Vector3 qx = Rotate((*this), Vector3(1.0f, 0.0f, 0.0f));
	Vector3 qy = Rotate((*this), Vector3(0.0f, 1.0f, 0.0f));
	Vector3 qz = Rotate((*this), Vector3(0.0f, 0.0f, 1.0f));

	return Mq;
}

void Quaternion::Normalize()
{
	float mag = Magnitude(*this);
	if (mag != 0.0f)
	{
		Quaternion numerator = *this;
		float denominator = mag;
		Quaternion div = *this / mag;
		this->Scalar = div.Scalar;
		this->Vector = div.Vector;
	}
}

Vector3 Quaternion::GetEulerAngles()
{
	Quaternion q = (*this);

	float s = q.Scalar;
	float x = q.Vector.x;
	float y = q.Vector.y;
	float z = q.Vector.z;

	float pitch_x = glm::atan(2.0f * (y*z + s*x), s*s - x*x - y*y + z*z);
	float yaw_y = glm::asin(glm::clamp((-2.0f) * (x*z - s*y), -1.0f, 1.0f));
	float roll_z = glm::atan(2.0f * (x*y + s*z), s*s + x*x - y*y - z*z);
	return Vector3(pitch_x, yaw_y, roll_z);
}

float Quaternion::GetCurrentAngle()
{
	float angle = 2.0f * glm::acos(this->Scalar);
	return angle;
}

Vector3 Quaternion::GetCurrentAxis()
{
	Vector3 A = this->Vector / glm::sin(GetCurrentAngle() * 0.5f);
	return A;
}


Quaternion Quaternion::Slerp(Quaternion from_q0, Quaternion to_q1, float u)
{
	Quaternion NewQuaternion;

	// compute angle
	float cosine = Quaternion::Dot(from_q0, to_q1);

	// used this method to make sure that 
	// the quaternion direction is always the
	// same. 
	int flag = 1;
	if (cosine < 0.0f)
	{
		flag = -1;
		cosine = -cosine;
	}
		
	float epsilon = 0.0000001f;
	if ((1.0f - cosine) > epsilon)
	{
		// If angle different than zero, slerp is applied.
		float angle = glm::acos(cosine);
		NewQuaternion =
			( glm::sin(angle - u*angle)*from_q0 + (flag * sin(u*angle)*to_q1) ) / sin(angle);
	}
	else
	{
		// If angle is almost zero a NaN will result.
		// However, smoothnes is still required,
		// so linear interpolation is applied.
		NewQuaternion = (1 - u)*from_q0 + flag*u*to_q1;
	}


	
	return NewQuaternion;
}

Quaternion Quaternion::LookAt(Vector3 position, Vector3 target)
{
	Quaternion rotator;
	assert(target != position);

	Vector3 direction = glm::normalize(target - position);
	float dot = glm::dot(Vector3(0.0f, 0.0f, 1.0f), direction);
	if (glm::abs(dot - (-1.0f)) < 0.0000001f)
	{
		rotator = Quaternion::QuatFromAxisAngle(glm::radians(180.0f), Vector3(0.0f, 1.0f, 0.0f));
		return rotator;
	}
	else if(glm::abs(dot - 1.0f) < 0.0000001f)
	{
		rotator = Quaternion();
		return rotator;
	}

	float angle = -glm::acos(dot);
	Vector3 cross = 
		glm::normalize(glm::cross(Vector3(0.0f, 0.0f, 1.0f), direction));
	
	rotator = Quaternion::QuatFromAxisAngle(angle, cross);
	rotator.Normalize();

	return rotator;
}

Quaternion Quaternion::QuatFromMatrix(Matrix4* rotation_matrix)
{
	Quaternion rotator;
	Matrix4 mat = *rotation_matrix;

	float trace = mat[0][0] + mat[1][1] + mat[2][2];

	if (trace > 0.0f)
	{
		rotator.Scalar = 0.5f * glm::sqrt(1.0f + mat[0][0] +
			mat[1][1] + mat[2][2]);

		rotator.Vector.x = mat[2][1] - mat[1][2];
		rotator.Vector.y = mat[0][2] - mat[2][0];
		rotator.Vector.z = mat[1][0] - mat[0][1];
		rotator.Vector = rotator.Vector / (4 * rotator.Scalar);
	}
	else if ((mat[0][0] > mat[1][1]) && (mat[0][0] > mat[2][2]))
	{
		float S = 0.5f * glm::sqrt(1.0f + mat[0][0] -
			mat[1][1] - mat[2][2]);
		float denom = 4 * S;
		rotator.Scalar = (mat[2][1] - mat[1][2]) / denom;
		rotator.Vector.x = S;
		rotator.Vector.y = (mat[1][0] + mat[0][1]) / denom;
		rotator.Vector.z = (mat[0][2] + mat[2][0]) / denom;
	}
	else if (mat[1][1] > mat[2][2])
	{
		float S = 0.5f * glm::sqrt(1.0f + mat[1][1] -
			mat[0][0] - mat[2][2]);
		float denom = 4 * S;

		rotator.Scalar = (mat[0][2] - mat[2][0]) / denom;
		rotator.Vector.x = (mat[1][0] + mat[0][1]) / denom;
		rotator.Vector.y = S;
		rotator.Vector.z = (mat[2][1] + mat[1][2]) / denom;
	}
	else
	{
		float S = 0.5f * glm::sqrt(1.0f + mat[2][2] -
			mat[0][0] - mat[1][1]);
		float denom = 4 * S;

		rotator.Scalar = (mat[1][0] - mat[0][1]) / denom;
		rotator.Vector.x = (mat[0][2] + mat[2][0]) / denom;
		rotator.Vector.y = (mat[2][1] + mat[1][2]) / denom;
		rotator.Vector.z = S;
	}

	

	return rotator;
}

// This implements the multiplication between Quaternion and Homogeneous Matrix 4x4  
// In the paper, it is the "." operator
Matrix4 Quaternion::Qprod(Quaternion q, Matrix4 HMatrix)
{
	Matrix4 result;
	Quaternion col1 = Quaternion(
		HMatrix[0][0], Vector3(HMatrix[1][0], HMatrix[2][0], HMatrix[3][0])
	);
	Quaternion col2 = Quaternion(
		HMatrix[0][1], Vector3(HMatrix[1][1], HMatrix[2][1], HMatrix[3][1])
	);
	Quaternion col3 = Quaternion(
		HMatrix[0][2], Vector3(HMatrix[1][2], HMatrix[2][2], HMatrix[3][2])
	);
	Quaternion col4 = Quaternion(
		HMatrix[0][3], Vector3(HMatrix[1][3], HMatrix[2][3], HMatrix[3][3])
	);

	col1 = q * col1;
	col2 = q * col2;
	col3 = q * col3;
	col4 = q * col4;

	result[0][0] = col1.Scalar;
	result[1][0] = col1.Vector.x;
	result[2][0] = col1.Vector.y;
	result[3][0] = col1.Vector.z;

	result[0][1] = col2.Scalar;
	result[1][1] = col2.Vector.x;
	result[2][1] = col2.Vector.y;
	result[3][1] = col2.Vector.z;

	result[0][2] = col3.Scalar;
	result[1][2] = col3.Vector.x;
	result[2][2] = col3.Vector.y;
	result[3][2] = col3.Vector.z;

	result[0][3] = col4.Scalar;
	result[1][3] = col4.Vector.x;
	result[2][3] = col4.Vector.y;
	result[3][3] = col4.Vector.z;

	return result;
}

// This implements the multiplication between Homogeneous Matrix 4x4 and Quaternion
// In the paper, it is the "." operator
Matrix4 Quaternion::Qprod(Matrix4 HMatrix, Quaternion q)
{
	Matrix4 result;
	Quaternion col1 = Quaternion(
		HMatrix[0][0], Vector3(HMatrix[1][0], HMatrix[2][0], HMatrix[3][0])
	);
	Quaternion col2 = Quaternion(
		HMatrix[0][1], Vector3(HMatrix[1][1], HMatrix[2][1], HMatrix[3][1])
	);
	Quaternion col3 = Quaternion(
		HMatrix[0][2], Vector3(HMatrix[1][2], HMatrix[2][2], HMatrix[3][2])
	);
	Quaternion col4 = Quaternion(
		HMatrix[0][3], Vector3(HMatrix[1][3], HMatrix[2][3], HMatrix[3][3])
	);


	col1 = col1 * q;
	col2 = col2 * q;
	col3 = col3 * q;
	col4 = col4 * q;

	result[0][0] = col1.Scalar;
	result[1][0] = col1.Vector.x;
	result[2][0] = col1.Vector.y;
	result[3][0] = col1.Vector.z;

	result[0][1] = col2.Scalar;
	result[1][1] = col2.Vector.x;
	result[2][1] = col2.Vector.y;
	result[3][1] = col2.Vector.z;

	result[0][2] = col3.Scalar;
	result[1][2] = col3.Vector.x;
	result[2][2] = col3.Vector.y;
	result[3][2] = col3.Vector.z;

	result[0][3] = col4.Scalar;
	result[1][3] = col4.Vector.x;
	result[2][3] = col4.Vector.y;
	result[3][3] = col4.Vector.z;

	return result;
}