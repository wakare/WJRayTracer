#include<stdlib.h>
#include<iostream>
#include"Geometry.h"
#include"math.h"
#include"BmpOutput.h"

#define color __int32 

int main() 
{
	color* frameBuffer = (color*)malloc(sizeof(color) * WIDTH * HEIGHT);
	if (!frameBuffer)
		return 0;
	
	Render(frameBuffer, WIDTH, HEIGHT);
	saveBitmap(WIDTH, HEIGHT, (unsigned char*)frameBuffer, WIDTH * HEIGHT * 4);
	free(frameBuffer);
	return 0;
}