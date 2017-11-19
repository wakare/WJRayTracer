#pragma once

class color_t
{
public:
	color_t(float colorValue);
	float colorValue;

	float GetColorValue();

	// ReturnValue represent whether color value has overrange
	bool ColorAdd(color_t color);
	bool ColorModulate(color_t color);
	bool ColorMutiply(float k);
	bool ColorMerge(color_t mergeColor, float ratio);
	bool GammaCorrection();

	//Conventient calculate color value
	static color_t ColorModulate(color_t color1, color_t color2);
	static color_t ColorAdd(color_t color1, color_t color2);
	static color_t ColorMutiply(color_t color1, float k);
	static color_t ColorMerge(color_t sourceColor, color_t mergeColor, float ratio);
	static color_t GammaCorrection(color_t sourceColor);
};