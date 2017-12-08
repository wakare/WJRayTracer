#pragma once

#include "WJMath.h"

#define ColorMaskR 0x00ff0000
#define ColorMaskG 0x0000ff00
#define ColorMaskB 0x000000ff

class color_t
{
public:
	float colorValue;

	color_t(float colorValue);
	color_t(int colorValue);

	void operator=(float colorValue);
	void operator=(int colorValue);
	void operator=(color_t colorValue);
			
	float GetColorValue();
	float GetR();
	float GetG();
	float GetB();

	bool SetR(float rValue);
	bool SetG(float gValue);
	bool SetB(float bValue);

	bool SetRGB(float rValue, float gValue, float bValue);

	// ReturnValue represent whether color value has over range
	bool ColorAdd(color_t color);
	bool ColorModulate(color_t color);
	bool ColorMutiply(float k);
	bool ColorMerge(color_t mergeColor, float ratio);
	bool GammaCorrection();

	//Convenient calculate color value
	static color_t ColorModulate(color_t color1, color_t color2);
	static color_t ColorAdd(color_t color1, color_t color2);
	static color_t ColorMutiply(color_t color1, float k);
	static color_t ColorMerge(color_t sourceColor, color_t mergeColor, float ratio);
	static color_t GammaCorrection(color_t sourceColor);
};