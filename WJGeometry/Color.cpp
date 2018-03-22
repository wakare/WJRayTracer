#include "Color.h"
#include <assert.h>

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

Color_t::Color_t(int colorValue)
{
	this->m_colorValue = colorValue;
}

void Color_t::operator=(int colorValue)
{
	this->m_colorValue = colorValue;
}

void Color_t::operator=(const Color_t& colorValue)
{
	this->m_colorValue = colorValue.m_colorValue;
}

float Color_t::GetR()
{
	int nRValue = this->m_colorValue & ColorMaskR;
	nRValue = nRValue >> 16;
	return nRValue / 255.0f;
}

float Color_t::GetG()
{
	int nGValue = this->m_colorValue & ColorMaskG;
	nGValue = nGValue >> 8;
	return nGValue / 255.0f;
}

float Color_t::GetB()
{
	int nBValue = this->m_colorValue & ColorMaskB;
	return nBValue / 255.0f;
}

bool Color_t::SetR(float rValue)
{
	int nRValue = rValue * 255;
	if (0 <= nRValue && nRValue <= 255)
	{
		int colorGB = static_cast<int>(this->m_colorValue) & (ColorMaskB | ColorMaskG);
		int colorR = nRValue << 16;
		this->m_colorValue =  colorR | colorGB;

		return true;
	}
	return false;
}

bool Color_t::SetG(float gValue)
{
	int nGValue = gValue * 255;
	if (0 <= nGValue && nGValue <= 255)
	{
		int colorRB = static_cast<int>(this->m_colorValue) & (ColorMaskB | ColorMaskR);
		int colorG = nGValue << 8;
		this->m_colorValue = colorG | colorRB;

		return true;
	}
	return false;
}

bool Color_t::SetB(float bValue)
{
	int nBValue = bValue * 255;
	if (0 <= nBValue && nBValue <= 255)
	{
		int colorRG = static_cast<int>(this->m_colorValue) & (ColorMaskG | ColorMaskR);
		int colorB = nBValue;
		this->m_colorValue = colorB | colorRG;

		return true;
	}
	return false;
}

bool Color_t::SetRGB(float rValue, float gValue, float bValue)
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

	m_colorValue = colorResult;

	return overflow;
}

bool Color_t::ColorAdd(Color_t color)
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

	SetRGB(fRValue, fGValue, fBValue);

	return overflow;
}
bool Color_t::ColorModulate(Color_t color)
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

bool Color_t::ColorMutiply(float k)
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
bool Color_t::ColorMerge(Color_t mergeColor, float ratio)
{
	bool result = false;

	if (ratio > 1.0f || ratio < 0.0f)
		result = true;

	float fRValue = GetR() * (1 - ratio) + mergeColor.GetR() * ratio;
	float fGValue = GetG() * (1 - ratio) + mergeColor.GetG() * ratio;
	float fBValue = GetB() * (1 - ratio) + mergeColor.GetB() * ratio;

	SetRGB(fRValue, fGValue, fBValue);

	return result;
}

bool Color_t::GammaCorrection()
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
	
	return IsOverFlow;}

Color_t Color_t::ColorModulate(Color_t color1, Color_t color2)
{
	Color_t result = color1.m_colorValue;
	result.ColorModulate(color2);

	return result;
}

Color_t Color_t::ColorAdd(Color_t color1, Color_t color2)
{
	Color_t result = color1.m_colorValue;
	result.ColorAdd(color2);

	return result;
}

Color_t Color_t::ColorMutiply(Color_t color1, float k)
{
	Color_t result = color1.m_colorValue;
	result.ColorMutiply(k);

	return result;
}

Color_t Color_t::ColorMerge(Color_t sourceColor, Color_t mergeColor, float ratio)
{
	Color_t result = sourceColor.m_colorValue;
	result.ColorMerge(mergeColor,ratio);

	return result;
}

Color_t Color_t::GammaCorrection(Color_t sourceColor)
{
	Color_t result = sourceColor.m_colorValue;
	result.GammaCorrection();

	return result;
}
