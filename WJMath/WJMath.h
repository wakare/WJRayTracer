#pragma once

#include <math.h>

#define PI 3.1415926f
#define FLOAT_DEVIATION 1e-6f

class Matrix4 //4*4 matrix
{
public:
	float fMatrixArray[4][4];

	Matrix4 operator*(Matrix4&);
	Matrix4(float fMatrixArray[4][4]);
	Matrix4();
};

class Vector4
{
public:
	float fX;
	float fY;
	float fZ;
	float fW;

	Vector4&	operator*=(float);
	Vector4&	operator/=(float);
	Vector4&	operator=(const Vector4&);
	Vector4&	operator*(const Matrix4&);
	Vector4&	operator*=(const Matrix4&);
	Vector4&	operator+=(const Vector4&);
	
	Vector4		operator*(float);
	Vector4		operator+(const Vector4&);
	Vector4		operator-(const Vector4&);
	Vector4		operator-();
	Vector4		Cross(const Vector4&);
	float		operator*(Vector4);
	void		Normalize(); // reset (x,y,z,w) = (x/w,y/w,z/w,1.0f)
	void		ResetUnitVector();
	float		length();
};