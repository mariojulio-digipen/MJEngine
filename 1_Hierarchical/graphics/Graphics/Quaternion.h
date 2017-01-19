#pragma once
#include "glm.hpp"

typedef glm::vec3 Vector3;
typedef glm::mat4 Matrix4;
// *** This class has the following operations: ***
// o Addition (q1 + q2)
// o Scalar Multiplication (s * q)
// o Multiplication (q1 * q2)
// o Multiplication (q * M) ; M is Mat4
// o Multiplication (M * q) ; M is Mat4
// o Dot Product (q1.Dot(q2))
// o Magnitude (q1.Magnitude())
// o Conjugate (q1.Conjugate())
// o Inverse (q^-1)
// o Quaternions Division (q1 / q2)
// o Quaternion Divided by Scalar (q / s)
// o Identity (GetIdentity())
// o Normalize (Normalizes this quaternion)
class Quaternion
{
private:
	
	Quaternion addition(Quaternion q1, Quaternion q2);
	Quaternion scalarMultiplication(float s, Quaternion q);
	Quaternion multiplication(Quaternion q1, Quaternion q2);
	Quaternion multiplication(Matrix4 hmatrix, Quaternion q);
	Quaternion divide(Quaternion q1, Quaternion q2);
	Quaternion inverse(Quaternion q);
	Quaternion conjugate();
	
public:

	// I organized quaternions in two parts,
	// the scalar part and the vector part.
	// I think this way is more intuitive
	// for the programmer to retrieve
	// information from this class.
	float Scalar;
	Vector3 Vector;

	// creates an identity quaternion if no parameters are passed to the constructor
	Quaternion() : Scalar(1.0f), Vector((0.0f, 0.0f, 0.0f)) {} 
	// an arbitrary quaternion
	Quaternion(float scalar, Vector3 vector) : Scalar(scalar), Vector(vector) {}
	~Quaternion() {}

	Quaternion operator+(Quaternion const & rhs);
	Quaternion operator*(Quaternion const & rhs);



	friend Quaternion operator*(float const & lhs, Quaternion const & rhs);
	friend Quaternion operator*(Quaternion const & rhs, float const & lhs);
	friend Quaternion operator*(Vector3 lhs, Quaternion rhs);
	//friend Matrix4 operator.(Quaternion q, Matrix4 HMatrix);

	friend Quaternion operator*(Matrix4 lhs, Quaternion rhs);
	Quaternion operator/(Quaternion const & rhs);
	friend Quaternion operator/(Quaternion const & lhs, float const & rhs);
	Quaternion operator-();
	Quaternion operator^(int const & s);

	Matrix4 ToMatrix();
	void Normalize();
	Vector3 GetEulerAngles();
	float GetCurrentAngle();
	Vector3 GetCurrentAxis();

	static Vector3  RotateFast(float angle, Vector3 angle_axis, Vector3 vector_to_rotate);
	static Vector3  RotateFast2(float angle, Vector3 angle_axis, Vector3 vector_to_rotate);
	static Vector3 Rotate(float angle, Vector3 angle_axis, Vector3 vector_to_rotate);
	static Quaternion QuatFromAngles(Vector3 angle);
	static Vector3 Rotate(Quaternion q, Vector3 v);
	static Quaternion QuatFromAxisAngle(float angle, Vector3 axis);
	static Quaternion LookAt(Vector3 position, Vector3 target);
	static Quaternion Slerp(Quaternion from_q0, Quaternion to_q1, float u);
	static float Dot(Quaternion q1, Quaternion q2);
	static float Magnitude(Quaternion q);
	static Quaternion QuatFromMatrix(Matrix4* rotation_matrix);
	static Matrix4 Qprod(Quaternion q, Matrix4 HMatrix);
	static Matrix4 Qprod(Matrix4 HMatrix, Quaternion q);
	
	


};
