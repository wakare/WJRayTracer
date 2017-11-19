#include "WJMath.h"
#include <math.h>

int lineInterpolate(int start, int end, float k)
{
	return (int)(start + k*(end - start) + 0.5f);
}

void Vector4::ResetUnitVector()
{
	if (w > FLOAT_DEVIATION || w < -FLOAT_DEVIATION)
	{
		return;
	}
	float length = x*x + y*y + z*z;
	if (length > FLOAT_DEVIATION)
	{
		length = sqrtf(length);
		x = x / length;
		y = y / length;
		z = z / length;
	}
}

float Vector4::length()
{
	if (w < FLOAT_DEVIATION && w > -FLOAT_DEVIATION)
	{
		return 0.0f;
	}
	float length = x*x + y*y + z*z;
	length = sqrtf(length);
	return length;
}

void Vector4::Normalize()
{
	x = x / w;
	y = y / w;
	z = z / w;
	w = 1.0;
}

Vector4 Vector4::Cross(Vector4 _V)
{
	// only use for direction vector
	Vector4 result;
	result.w = 0;
	result.x = y*_V.z - z*_V.y;
	result.y = z*_V.x - x*_V.z;
	result.z = x*_V.y - y*_V.x;
	return result;
}

Vector4 & Vector4::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

Vector4 & Vector4::operator=(Vector4& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}

Vector4 Vector4::operator*(Matrix4 _M)
{
	Vector4 result;
	result.x = x*_M.M[0][0] + y*_M.M[1][0] + z*_M.M[2][0] + w*_M.M[3][0];
	result.y = x*_M.M[0][1] + y*_M.M[1][1] + z*_M.M[2][1] + w*_M.M[3][1];
	result.z = x*_M.M[0][2] + y*_M.M[1][2] + z*_M.M[2][2] + w*_M.M[3][2];
	result.w = x*_M.M[0][3] + y*_M.M[1][3] + z*_M.M[2][3] + w*_M.M[3][3];
	return result;
}

Vector4 & Vector4::operator*=(Matrix4 _M)
{
	float _x = x, _y = y, _z = z, _w = w;
	x = _x*_M.M[0][0] + _y*_M.M[1][0] + _z*_M.M[2][0] + _w*_M.M[3][0];
	y = _x*_M.M[0][1] + _y*_M.M[1][1] + _z*_M.M[2][1] + _w*_M.M[3][1];
	z = _x*_M.M[0][2] + _y*_M.M[1][2] + _z*_M.M[2][2] + _w*_M.M[3][2];
	w = _x*_M.M[0][3] + _y*_M.M[1][3] + _z*_M.M[2][3] + _w*_M.M[3][3];
	return *(this);
}

Vector4 & Vector4::operator+=(Vector4 v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

Vector4 & Vector4::operator/=(float f)
{
	float _f = 1.0f / f;
	x *= _f;
	y *= _f;
	z *= _f;
	w *= _f;
	return *this;

}

Vector4 Vector4::operator*(float f)
{
	Vector4 result;
	result.x = x * f;
	result.y = y * f;
	result.z = z * f;
	result.w = w;
	return result;
}

Vector4 Vector4::operator+(Vector4 v)
{
	Vector4 result;
	result.x = x + v.x;
	result.y = y + v.y;
	result.z = z + v.z;
	if (v.w < FLOAT_DEVIATION && v.w > -FLOAT_DEVIATION)
		result.w = 0.0f;
	else
		result.w = 1.0f;
	return result;
}

float Vector4::operator*(Vector4 _V)
{
	if (w > FLOAT_DEVIATION || w < -FLOAT_DEVIATION)
		Normalize();
	if (_V.w > FLOAT_DEVIATION || _V.w < -FLOAT_DEVIATION)
		_V.Normalize();
	return (x*_V.x + y*_V.y + z*_V.z);
}

Vector4 Vector4::operator- (Vector4 v)
{
	Vector4 result;
	result.x = x - v.x;
	result.y = y - v.y;
	result.z = z - v.z;
	result.w = 0;
	return result;
}

Vector4 Vector4::operator-()
{
	Vector4 result = Vector4();
	result.x = -x;
	result.y = -y;
	result.z = -z;
	return result;
}

Matrix4 Matrix4::operator*(Matrix4 _M)
{
	float resultArray[4][4] = { 0 };
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			resultArray[i][j] = M[i][0] * _M.M[0][j] + M[i][1] * _M.M[1][j] + M[i][2] * _M.M[2][j] + M[i][3] * _M.M[3][j];
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
				M[i][j] = Array[i][j];
	else
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				M[i][j] = 0.0f;
}

Matrix4::Matrix4()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			M[i][j] = 0.0f;
		}
	}
}


Matrix4& getPerspectiveProjMatrix(float theta, float zn, float zf, float widthHeightScale)
{
	//theta -- the view frustum angle(up to down)
	//zn -- near Z clipPlane
	//zf -- far Z clipPlane
	//widthHeightScale -- the value of Width / Height

	//step1. proj the point to znPlane.
	//suppose the point(x,y) ,we try to find correct value of x,y when the point are projed into projPlane.
	//p'.x/p.x = n/p.z		p'.y/p.y = n/p.z	=> p'.x = n * p.x/p.z		p'.y = n * p.y/p.z

	//step2. proj znPlane point to a standard plane(x'',y'' is [-1,1])
	//p'.x/p''.x = W/2		p'.y/p''.y = H/2	=> p''.x = 2 * p'.x/W		p''.y = 2 * p'.y/H
	//p''.x = 2*n*p.x/W*p.z		p''.y = 2*n*p.y/H*p.z
	//cot(theta) = 2n/H ==> p''.y = cot(theta)*p.y/p.z		p''.x / p''.y = (p.x/p.y)*(H/W) ==> p''.x = p.x*cot(theta)/p.z*widthHeightScale

	//last step is to find z-value after projection(z'' is [0,1])
	//find a linear transform equation feds z'' value is small when the object is nearer than which z'' value is larger.
	//a * 1/n + b = 0		a * 1/f + b = 1	=>	-n * b/f + b = 1	=>	b = 1/(1 - n/f)	a = n/(n/f - 1)
	//the finally z value = n/(n/f - 1)/z + 1/(1 - n/f)
	//we get the proj position(p.x,p.y,p.z) => ((p.x/p.z)*cot(theta)/widthHeightScale,cot(theta)*p.y/p.z,nf/(n - f)*p.z + f/(f - n))
	//ProjMatrix feds equation (p.x,p.y,p.z,1) * ProjMatrix = (p''.x,p''.y,p''.z,1)
	//we can calculate the matrix is :
	float ProjArray[4][4] =
	{
		1 / (tan(theta / 2)*widthHeightScale),	0,						0,						0,
		0,										1 / tan(theta / 2),		0,						0,
		0,										0,						zf / (zf - zn),			1,
		0,										0,						zf*zn / (zn - zf),		0
	};
	Matrix4 *ProjMatrix = new Matrix4(ProjArray);
	return *ProjMatrix;
}

Matrix4& getViewPortMatrix(float x, float y, float width, float height, float minZ, float maxZ)
{
	// CVV vertex -> Viewport position
	// use several special position to calculate the viewport matrix.
	// (-1,1,0,1)	->	(x,y,minZ,1)
	// (1,1,0,1)	->	(x + width,y,minZ,1)
	// (-1,-1,1,1)	->	(x,y+height,maxZ,1)
	// (1,-1,1,1)	->	(x+width,y+height,maxZ,1)
	float viewportArray[4][4] =
	{
		width / 2,		0,					0,				0,
		0,				-height / 2,		0,				0,
		0,				0,					maxZ - minZ,	0,
		x + width / 2,	y + height / 2,		minZ,			1
	};

	Matrix4* viewportMatrix = new Matrix4(viewportArray);
	return *viewportMatrix;
}
