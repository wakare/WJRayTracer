#pragma once

#include "WJMath.h"
#include <stdint.h>

#define ColorMaskR 0x00ff0000
#define ColorMaskG 0x0000ff00
#define ColorMaskB 0x000000ff

class Color_t
{
public:
    int32_t m_colorValue;

	Color_t(int colorValue);

	void operator=(int colorValue);
	void operator=(const Color_t& colorValue);
			
	float GetR();
	float GetG();
	float GetB();

	bool SetR(float rValue);
	bool SetG(float gValue);
	bool SetB(float bValue);

	bool SetRGB(float rValue, float gValue, float bValue);

	// ReturnValue represent whether color value has over range
	bool ColorAdd(Color_t color);
	bool ColorModulate(Color_t color);
	bool ColorMutiply(float k);
	bool ColorMerge(Color_t mergeColor, float ratio);
	bool GammaCorrection();

	// Convenient calculate color value
	static Color_t ColorModulate(Color_t color1, Color_t color2);
	static Color_t ColorAdd(Color_t color1, Color_t color2);
	static Color_t ColorMutiply(Color_t color1, float k);
	static Color_t ColorMerge(Color_t sourceColor, Color_t mergeColor, float ratio);
	static Color_t GammaCorrection(Color_t sourceColor);
};