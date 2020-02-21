#ifndef PGM_H
#define PGM_H

#define uchar unsigned char

typedef struct PGM{
	char header[2];
	int width, height, maxValue;
	uchar* data;
}PGM;

PGM* readPGM(char* fileName);
void rotatePGM_L(PGM* pgm);
void rotatePGM_R(PGM* pgm);
void inversePGM(PGM* pgm);
void reflectPGM_V(PGM* pgm);
void reflectPGM_H(PGM* pgm);
void writePGM(char* fileName, PGM* pgm);

#endif