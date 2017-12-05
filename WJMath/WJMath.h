#pragma once

#include <math.h>

#define PI 3.1415926f
#define FLOAT_DEVIATION 1e-6f

class Matrix4 //4*4 matrix
{
public:
	float M[4][4];
	Matrix4 operator*(Matrix4);
	Matrix4(float Array[4][4]);
	Matrix4();
};

class Vector4
{
public:
	float x;
	float y;
	float z;
	float w;
	Vector4& operator*=(float);
	Vector4& operator=(Vector4&);
	Vector4 operator*(Matrix4);
	Vector4& operator*=(Matrix4);
	Vector4& operator+=(Vector4);
	Vector4& operator/=(float);
	float operator*(Vector4);
	Vector4 operator*(float);
	Vector4 operator+(Vector4);
	Vector4 operator-(Vector4);
	Vector4 operator-();
	Vector4 Cross(Vector4);
	void Normalize(); // reset (x,y,z,w) = (x/w,y/w,z/w,1.0f)
	void ResetUnitVector();
	float length();
};