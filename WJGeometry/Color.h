#pragma once

#include "WJMath.h"
#include <stdint.h>
#include <stdlib.h>
#include <type_traits>

#define ColorMaskR 0x00ff0000
#define ColorMaskG 0x0000ff00
#define ColorMaskB 0x000000ff

enum class COLOR_TYPE
{
	RED		= 0x00FF0000,
	GREEN	= 0x0000FF00,
	BLUE	= 0x000000FF,
	WHITE	= 0x00FFFFFF,
	BLACK	= 0x00000000
};

class Color_t
{
public:
    int32_t m_colorValue;

	Color_t(int colorValue);
	Color_t(COLOR_TYPE color) 
	{ 
		using _COLOR_TYPE = std::underlying_type_t<COLOR_TYPE>;
		m_colorValue = static_cast<_COLOR_TYPE>(color);
	};

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