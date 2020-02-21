#ifndef PPM_H
#define PPM_H

#define uchar unsigned char

typedef struct PIXEL{
    uchar r, g, b;
}PIXEL;

typedef struct PPM{
	char header[2];
	int width, height, maxValue;
	PIXEL* data;
}PPM;

PPM* readPPM(char* fileName);
void rotatePPM_L(PPM* ppm);
void rotatePPM_R(PPM* ppm);
void inversePPM(PPM* ppm);
void reflectPPM_V(PPM* ppm);
void reflectPPM_H(PPM* ppm);
void writePPM(char* fileName, PPM* ppm);

#endif