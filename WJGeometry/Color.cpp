#include "Color.h"

inline float _GetRangeValue(float source, float smallest, float largest)
{
	if (source < smallest)
		source = smallest;

	if (source > largest)
		source = largest;

	return source;
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
		this->colorValue = colorR | colorGB;

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
		this->colorValue = colorG | colorRB;

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
		this->colorValue = colorB | colorRG;

		return true;
	}
	return false;
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
	this->colorValue = static_cast<float>(colorResult);

	return overflow;
}
