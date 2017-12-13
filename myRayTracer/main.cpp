#include<stdlib.h>
#include<iostream>
#include"Geometry.h"
#include"math.h"
#include"BmpOutput.h"

int main() 
{
	color_t* frameBuffer = (color_t*)malloc(sizeof(color_t) * WIDTH * HEIGHT);
	if (!frameBuffer)
	{
		return 0;
	}
	Render(frameBuffer, WIDTH, HEIGHT);
	saveBitmap(WIDTH, HEIGHT, (unsigned char*)frameBuffer, WIDTH * HEIGHT * 4);
	free(frameBuffer);
	return 0;
}