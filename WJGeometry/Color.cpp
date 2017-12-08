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

color_t::color_t(int colorValue)
{
	this->colorValue = _ConvertIntToFloat(colorValue);
}

void color_t::operator=(float colorValue)
{
	this->colorValue = colorValue;
}

void color_t::operator=(int colorValue)
{
	this->colorValue = _ConvertIntToFloat(colorValue);
}

void color_t::operator=(color_t colorValue)
{
	this->colorValue = colorValue.colorValue;
}

float color_t::GetColorValue()
{
	return colorValue;
}

float color_t::GetR()
{
	int nRValue = static_cast<int>(this->colorValue) & ColorMaskR;
	return nRValue / 255.0f;
}

float color_t::GetG()
{
	int nGValue = static_cast<int>(this->colorValue) & ColorMaskG;
	return nGValue / 255.0f;
}

float color_t::GetB()
{
	int nBValue = static_cast<int>(this->colorValue) & ColorMaskB;
	return nBValue / 255.0f;
}

bool color_t::SetR(float rValue)
{
	int nRValue = rValue * 255;
	if (0 <= nRValue && nRValue <= 255)
	{
		int colorGB = static_cast<int>(this->colorValue) & (ColorMaskB | ColorMaskG);
		int colorR = nRValue << 16;
		this->colorValue = _ConvertIntToFloat( colorR | colorGB);

		return true;
	}
	return false;
}

bool color_t::SetG(float gValue)
{
	int nGValue = gValue * 255;
	if (0 <= nGValue && nGValue <= 255)
	{
		int colorRB = static_cast<int>(this->colorValue) & (ColorMaskB | ColorMaskR);
		int colorG = nGValue << 16;
		this->colorValue = _ConvertIntToFloat(colorG | colorRB);

		return true;
	}
	return false;
}

bool color_t::SetB(float bValue)
{
	int nBValue = bValue * 255;
	if (0 <= nBValue && nBValue <= 255)
	{
		int colorRG = static_cast<int>(this->colorValue) & (ColorMaskG | ColorMaskR);
		int colorB = nBValue << 16;
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

	int nRValue = rValue * 255.0f;
	int nGValue = gValue * 255.0f;
	int nBValue = bValue * 255.0f;
	
	int colorResult = (nRValue << 16) | (nGValue << 8) | (nBValue);

	colorValue = _ConvertIntToFloat(colorResult);

	return overflow;
}

bool color_t::ColorAdd(color_t color)
{
	bool overflow = false;

	float fRValue = this->GetR() + color.GetR();
	float fGValue = this->GetG() + color.GetG();
	float fBValue = this->GetB() + color.GetB();

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

	int nRValue = fRValue * 255.0f;
	int nGValue = fGValue * 255.0f;
	int nBValue = fBValue * 255.0f;

	int colorResult = (nRValue << 16) | (nGValue << 8) | (nBValue);
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
	
	float fRValue = GetR() * k;
	float fGValue = GetG() * k;
	float fBValue = GetB() * k;

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

bool color_t::ColorMerge(color_t mergeColor, float ratio)
{
	float fRValue = GetR() * (1 - ratio) + mergeColor.GetR() * ratio;
	float fGValue = GetG() * (1 - ratio) + mergeColor.GetG() * ratio;
	float fBValue = GetB() * (1 - ratio) + mergeColor.GetB() * ratio;

	SetRGB(fRValue, fGValue, fBValue);
}

bool color_t::GammaCorrection()
{
	bool IsOverFlow = false;
	const float GammaRatio = 2.2f;

	float fRValue = GetR();
	float fGValue = GetG();
	float fBValue = GetB();

	if (fRValue > 1.0f || fRValue < 0.0f)
		IsOverFlow = true;

	if (fGValue > 1.0f || fGValue < 0.0f)
		IsOverFlow = true;

	if (fBValue > 1.0f || fBValue < 0.0f)
		IsOverFlow = true;

	fRValue = pow(fRValue, 1 / GammaRatio);
	fGValue = pow(fGValue, 1 / GammaRatio);
	fBValue = pow(fBValue, 1 / GammaRatio);

	SetRGB(fRValue, fGValue, fBValue);
	
	return IsOverFlow;
}

color_t color_t::ColorModulate(color_t color1, color_t color2)
{
	color_t result = color1.colorValue;
	result.ColorModulate(color2);

	return result;
}

color_t color_t::ColorAdd(color_t color1, color_t color2)
{
	color_t result = color1.colorValue;
	result.ColorAdd(color2);

	return result;
}

color_t color_t::ColorMutiply(color_t color1, float k)
{
	color_t result = color1.colorValue;
	result.ColorMutiply(k);

	return result;
}

color_t color_t::ColorMerge(color_t sourceColor, color_t mergeColor, float ratio)
{
	color_t result = sourceColor.colorValue;
	result.ColorMerge(mergeColor,ratio);

	return result;
}

color_t color_t::GammaCorrection(color_t sourceColor)
{
	color_t result = sourceColor.colorValue;
	result.GammaCorrection();

	return result;
}
