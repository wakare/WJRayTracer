#include "Color.h"

color_t ColorAdd(color_t color1, color_t color2)
{
	color_t result = 0x0;
	color_t R1 = CMID((color1 & 0x00FF0000) >> 16);
	color_t G1 = CMID((color1 & 0x0000FF00) >> 8);
	color_t B1 = CMID((color1 & 0x000000FF));
	color_t R2 = CMID((color2 & 0x00FF0000) >> 16);
	color_t G2 = CMID((color2 & 0x0000FF00) >> 8);
	color_t B2 = CMID((color2 & 0x000000FF));
	color_t R = CMID(R1 + R2);
	color_t G = CMID(G1 + G2);
	color_t B = CMID(B1 + B2);
	result = (R << 16) | (G << 8) | B;
	return result;
}
