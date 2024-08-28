#ifndef LAB3_PGM_H
#define LAB3_PGM_H

#include <vector>
#include <cstdio>
#include <cstring>

typedef unsigned char uchar;

class PGM{
private:
    char header[2];
    int width, height;
    uchar maxValue;
    uchar* data;
    double* errors;
public:
    explicit PGM (char* fileName, bool gradient, double gamma = 0);
    ~PGM();
    void print(char* fileName, int bit, bool gradient, double gamma = 0);
    void dither(int bit, int typeOfDithering, double gamma = 0);
};

#endif //LAB3_PGM_H
