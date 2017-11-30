#include "Color.h"
#include <assert.h>

inline float _ConvertIntToFloat(int sourceValue)
{
	float* pResult = reinterpret_cast<float*>(&sourceValue);
	return *(pResult);
}

inline float _GetRangeValue(float source, float smallest, float largest)
{
	assert(smallest < largest);

	if (source < smallest)
	{
		source = smallest;
		goto Exit0;
	}

	if (source > largest)
	{
		source = largest;
		goto Exit0;
	}

Exit0:
	return source;
}

color_t::color_t(float colorValue)
{
	this->colorValue = colorValue;
}

float color_t::GetColorValue()
{
	return colorValue;
}

float color_t::GetR()
{
	int rValue = static_cast<int>(this->colorValue) & ColorMaskR;
	return rValue / 255.0f;
}

float color_t::GetG()
{
	int gValue = static_cast<int>(this->colorValue) & ColorMaskG;
	return gValue / 255.0f;
}

float color_t::GetB()
{
	int bValue = static_cast<int>(this->colorValue) & ColorMaskB;
	return bValue / 255.0f;
}

bool color_t::SetR(float rValue)
{
	int r = rValue * 255;
	if (0 <= r && r <= 255)
	{
		int colorGB = static_cast<int>(this->colorValue) & (ColorMaskB | ColorMaskG);
		int colorR = r << 16;
		this->colorValue = _ConvertIntToFloat( colorR | colorGB);

		return true;
	}
	return false;
}

bool color_t::SetG(float gValue)
{
	int g = gValue * 255;
	if (0 <= g && g <= 255)
	{
		int colorRB = static_cast<int>(this->colorValue) & (ColorMaskB | ColorMaskR);
		int colorG = g << 16;
		this->colorValue = _ConvertIntToFloat(colorG | colorRB);

		return true;
	}
	return false;
}

bool color_t::SetB(float bValue)
{
	int b = bValue * 255;
	if (0 <= b && b <= 255)
	{
		int colorRG = static_cast<int>(this->colorValue) & (ColorMaskG | ColorMaskR);
		int colorB = b << 16;
		this->colorValue = _ConvertIntToFloat (colorB | colorRG);

		return true;
	}
	return false;
}

bool color_t::SetRGB(float rValue, float gValue, float bValue)
{
	bool overflow = false;

	if (rValue > 1.0f || rValue < 0.0f)
	{
		overflow = true;
		rValue = _GetRangeValue(rValue, 0.0f, 1.0f);
	}

	if (gValue > 1.0f || gValue < 0.0f)
	{
		overflow = true;
		gValue = _GetRangeValue(gValue, 0.0f, 1.0f);
	}

	if (bValue > 1.0f || bValue < 0.0f)
	{
		overflow = true;
		bValue = _GetRangeValue(bValue, 0.0f, 1.0f);
	}

	int r = rValue * 255.0f;
	int g = gValue * 255.0f;
	int b = bValue * 255.0f;
	
	int colorResult = (r << 16) | (g << 8) | (b);

	colorValue = _ConvertIntToFloat(colorResult);

	return overflow;
}

bool color_t::ColorAdd(color_t color)
{
	bool overflow = false;

	float rValue = this->GetR() + color.GetR();
	float gValue = this->GetG() + color.GetG();
	float bValue = this->GetB() + color.GetB();

	if (rValue > 1.0f || rValue < 0.0f)
	{
		overflow = true;
		rValue = _GetRangeValue(rValue, 0.0f, 1.0f);
	}

	if (gValue > 1.0f || gValue < 0.0f)
	{
		overflow = true;
		gValue = _GetRangeValue(gValue, 0.0f, 1.0f);
	}

	if (bValue > 1.0f || bValue < 0.0f)
	{
		overflow = true;
		bValue = _GetRangeValue(bValue, 0.0f, 1.0f);
	}

	int r = rValue * 255.0f;
	int g = gValue * 255.0f;
	int b = bValue * 255.0f;

	int colorResult = (r << 16) | (g << 8) | (b);
	this->colorValue = _ConvertIntToFloat(colorResult);

	return overflow;
}
bool color_t::ColorModulate(color_t color)
{
	bool overflow = false;

	float fRValue = GetR()*color.GetR();
	float fGValue = GetG()*color.GetG();
	float fBValue = GetB()*color.GetB();

	if (fRValue > 1.0f || fRValue < 0.0f)
	{
		overflow = true;
		fRValue = _GetRangeValue(fRValue, 0.0f, 1.0f);
	}

	if (fGValue > 1.0f || fGValue < 0.0f)
	{
		overflow = true;
		fGValue = _GetRangeValue(fGValue, 0.0f, 1.0f);
	}

	if (fBValue > 1.0f || fBValue < 0.0f)
	{
		overflow = true;
		fBValue = _GetRangeValue(fBValue, 0.0f, 1.0f);
	}

	SetRGB(fRValue, fGValue, fBValue);

	return overflow;
}

bool color_t::ColorMutiply(float k)
{
	bool overflow = false;
	
	float rValue = GetR() * k;
	float gValue = GetG() * k;
	float bValue = GetB() * k;

	if (rValue > 1.0f || rValue < 0.0f)
	{
		overflow = true;
		rValue = _GetRangeValue(rValue, 0.0f, 1.0f);
	}

	if (gValue > 1.0f || gValue < 0.0f)
	{
		overflow = true;
		gValue = _GetRangeValue(gValue, 0.0f, 1.0f);
	}

	if (bValue > 1.0f || bValue < 0.0f)
	{
		overflow = true;
		bValue = _GetRangeValue(bValue, 0.0f, 1.0f);
	}

	int r = rValue * 255.0f;
	int g = gValue * 255.0f;
	int b = bValue * 255.0f;

	int colorResult = (r<<16) | (g<<8) | (b);
	colorValue = _ConvertIntToFloat(colorResult);

	return overflow;
}

