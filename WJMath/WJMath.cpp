#include "WJMath.h"
#include <math.h>

inline int lineInterpolate(int start, int end, float k)
{
	return (int)(start + k*(end - start) + 0.5f);
}

void Vector4::ResetUnitVector()
{
	if (fW > FLOAT_DEVIATION || fW < -FLOAT_DEVIATION)
	{
		return;
	}
	float length = fX*fX + fY*fY + fZ*fZ;
	if (length > FLOAT_DEVIATION)
	{
		length = sqrtf(length);
		fX = fX / length;
		fY = fY / length;
		fZ = fZ / length;
	}
}

float Vector4::length()
{
	if (fW < FLOAT_DEVIATION && fW > -FLOAT_DEVIATION)
	{
		return 0.0f;
	}
	float length = fX*fX + fY*fY + fZ*fZ;
	length = sqrtf(length);
	return length;
}

void Vector4::Normalize()
{
	fX = fX / fW;
	fY = fY / fW;
	fZ = fZ / fW;
	fW = 1.0;
}

Vector4 Vector4::Cross(const Vector4& _V)
{
	// only use for direction vector
	Vector4 result;
	result.fW = 0;
	result.fX = fY*_V.fZ - fZ*_V.fY;
	result.fY = fZ*_V.fX - fX*_V.fZ;
	result.fZ = fX*_V.fY - fY*_V.fX;
	return result;
}

Vector4 & Vector4::operator*=(float f)
{
	fX *= f;
	fY *= f;
	fZ *= f;
	return *this;
}

Vector4 & Vector4::operator=(const Vector4& v)
{
	fX = v.fX;
	fY = v.fY;
	fZ = v.fZ;
	fW = v.fW;
	return *this;
}

Vector4& Vector4::operator*(const Matrix4& _M)
{
	Vector4 result;
	result.fX = fX*_M.fMatrixArray[0][0] + fY*_M.fMatrixArray[1][0] + fZ*_M.fMatrixArray[2][0] + fW*_M.fMatrixArray[3][0];
	result.fY = fX*_M.fMatrixArray[0][1] + fY*_M.fMatrixArray[1][1] + fZ*_M.fMatrixArray[2][1] + fW*_M.fMatrixArray[3][1];
	result.fZ = fX*_M.fMatrixArray[0][2] + fY*_M.fMatrixArray[1][2] + fZ*_M.fMatrixArray[2][2] + fW*_M.fMatrixArray[3][2];
	result.fW = fX*_M.fMatrixArray[0][3] + fY*_M.fMatrixArray[1][3] + fZ*_M.fMatrixArray[2][3] + fW*_M.fMatrixArray[3][3];
	return result;
}

Vector4 & Vector4::operator*=(const Matrix4& _M)
{
	float _x = fX, _y = fY, _z = fZ, _w = fW;
	fX = _x*_M.fMatrixArray[0][0] + _y*_M.fMatrixArray[1][0] + _z*_M.fMatrixArray[2][0] + _w*_M.fMatrixArray[3][0];
	fY = _x*_M.fMatrixArray[0][1] + _y*_M.fMatrixArray[1][1] + _z*_M.fMatrixArray[2][1] + _w*_M.fMatrixArray[3][1];
	fZ = _x*_M.fMatrixArray[0][2] + _y*_M.fMatrixArray[1][2] + _z*_M.fMatrixArray[2][2] + _w*_M.fMatrixArray[3][2];
	fW = _x*_M.fMatrixArray[0][3] + _y*_M.fMatrixArray[1][3] + _z*_M.fMatrixArray[2][3] + _w*_M.fMatrixArray[3][3];
	return *(this);
}

Vector4 & Vector4::operator+=(const Vector4& v)
{
	fX += v.fX;
	fY += v.fY;
	fZ += v.fZ;
	fW += v.fW;
	return *this;
}

Vector4 & Vector4::operator/=(float f)
{
	float _f = 1.0f / f;
	fX *= _f;
	fY *= _f;
	fZ *= _f;
	fW *= _f;
	return *this;

}

Vector4 Vector4::operator*(float f)
{
	Vector4 result;
	result.fX = fX * f;
	result.fY = fY * f;
	result.fZ = fZ * f;
	result.fW = fW;
	return result;
}

Vector4 Vector4::operator+(const Vector4& v)
{
	Vector4 result;
	result.fX = fX + v.fX;
	result.fY = fY + v.fY;
	result.fZ = fZ + v.fZ;
	if (v.fW < FLOAT_DEVIATION && v.fW > -FLOAT_DEVIATION)
		result.fW = 0.0f;
	else
		result.fW = 1.0f;
	return result;
}

float Vector4::operator*(Vector4 _V)
{
	if (fW > FLOAT_DEVIATION || fW < -FLOAT_DEVIATION)
		Normalize();
	if (_V.fW > FLOAT_DEVIATION || _V.fW < -FLOAT_DEVIATION)
		_V.Normalize();
	return (fX*_V.fX + fY*_V.fY + fZ*_V.fZ);
}

Vector4 Vector4::operator- (const Vector4& v)
{
	Vector4 result;
	result.fX = fX - v.fX;
	result.fY = fY - v.fY;
	result.fZ = fZ - v.fZ;
	result.fW = 0;
	return result;
}

Vector4 Vector4::operator-()
{
	Vector4 result;

	result.fX = -fX;
	result.fY = -fY;
	result.fZ = -fZ;

	return result;
}

Matrix4 Matrix4::operator*(Matrix4& _M)
{
	float resultArray[4][4] = { 0 };
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			resultArray[i][j] = fMatrixArray[i][0] * _M.fMatrixArray[0][j] + fMatrixArray[i][1] * _M.fMatrixArray[1][j] + fMatrixArray[i][2] * _M.fMatrixArray[2][j] + fMatrixArray[i][3] * _M.fMatrixArray[3][j];
		}
	}
	Matrix4 result = Matrix4(resultArray);
	return result;
}

Matrix4::Matrix4(float Array[4][4])
{
	if (Array != NULL)
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				fMatrixArray[i][j] = Array[i][j];
	else
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				fMatrixArray[i][j] = 0.0f;
}

Matrix4::Matrix4()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			fMatrixArray[i][j] = 0.0f;
		}
	}
}

//Matrix4& getPerspectiveProjMatrix(float theta, float zn, float zf, float widthHeightScale)
//{
//	//theta -- the view frustum angle(up to down)
//	//zn -- near Z clipPlane
//	//zf -- far Z clipPlane
//	//widthHeightScale -- the value of Width / Height
//
//	//step1. proj the point to znplane->
//	//suppose the point(x,y) ,we try to find correct value of x,y when the point are projed into projplane->
//	//p'.x/p.x = n/p.z		p'.y/p.y = n/p.z	=> p'.x = n * p.x/p.z		p'.y = n * p.y/p.z
//
//	//step2. proj znPlane point to a standard plane(x'',y'' is [-1,1])
//	//p'.x/p''.x = W/2		p'.y/p''.y = H/2	=> p''.x = 2 * p'.x/W		p''.y = 2 * p'.y/H
//	//p''.x = 2*n*p.x/W*p.z		p''.y = 2*n*p.y/H*p.z
//	//cot(theta) = 2n/H ==> p''.y = cot(theta)*p.y/p.z		p''.x / p''.y = (p.x/p.y)*(H/W) ==> p''.x = p.x*cot(theta)/p.z*widthHeightScale
//
//	//last step is to find z-value after projection(z'' is [0,1])
//	//find a linear transform equation feds z'' value is small when the object is nearer than which z'' value is larger.
//	//a * 1/n + b = 0		a * 1/f + b = 1	=>	-n * b/f + b = 1	=>	b = 1/(1 - n/f)	a = n/(n/f - 1)
//	//the finally z value = n/(n/f - 1)/z + 1/(1 - n/f)
//	//we get the proj position(p.x,p.y,p.z) => ((p.x/p.z)*cot(theta)/widthHeightScale,cot(theta)*p.y/p.z,nf/(n - f)*p.z + f/(f - n))
//	//ProjMatrix feds equation (p.x,p.y,p.z,1) * ProjMatrix = (p''.x,p''.y,p''.z,1)
//	//we can calculate the matrix is :
//	float ProjArray[4][4] =
//	{
//		1 / (tan(theta / 2)*widthHeightScale),	0,						0,						0,
//		0,										1 / tan(theta / 2),		0,						0,
//		0,										0,						zf / (zf - zn),			1,
//		0,										0,						zf*zn / (zn - zf),		0
//	};
//	Matrix4 *ProjMatrix = new Matrix4(ProjArray);
//	return *ProjMatrix;
//}

//Matrix4 getViewPortMatrix(float x, float y, float width, float height, float minZ, float maxZ)
//{
//	// CVV vertex -> Viewport position
//	// use several special position to calculate the viewport matrix.
//	// (-1,1,0,1)	->	(x,y,minZ,1)
//	// (1,1,0,1)	->	(x + width,y,minZ,1)
//	// (-1,-1,1,1)	->	(x,y+height,maxZ,1)
//	// (1,-1,1,1)	->	(x+width,y+height,maxZ,1)
//	float viewportArray[4][4] =
//	{
//		width / 2,		0,					0,				0,
//		0,				-height / 2,		0,				0,
//		0,				0,					maxZ - minZ,	0,
//		x + width / 2,	y + height / 2,		minZ,			1
//	};
//
//	Matrix4 viewportMatrix(viewportArray);
//	return viewportMatrix;
//}
