#ifndef LAB4_PGM_H
#define LAB4_PGM_H

#include <vector>
#include <cstdio>
#include <cstring>

typedef unsigned char uchar;

struct Pixel{
    uchar first, second, third;
    Pixel(uchar first, uchar second, uchar third) : first(first), second(second), third(third) {}
    Pixel() = default;
};

class PPM{
private:
    char header[2];
    int width, height;
    uchar maxValue;
    Pixel* data;

    static Pixel convertRGBtoHSL(const Pixel& pixel);
    static Pixel convertHSLtoRGB(const Pixel& pixel);
    static Pixel convertHSLtoHSV(const Pixel& pixel);
    static Pixel convertHSVtoHSL(const Pixel& pixel);
    static Pixel convertRGBtoYCbCr_601(const Pixel& pixel);
    static Pixel convertRGBtoYCbCr_709(const Pixel& pixel);
    static Pixel convertYCbCr_601toRGB(const Pixel& pixel);
    static Pixel convertYCbCr_709toRGB(const Pixel& pixel);
    static Pixel convertRGBtoYCoCg(const Pixel& pixel);
    static Pixel convertYCoCgtoRGB(const Pixel& pixel);
    static Pixel convertRGBtoCMY(const Pixel& pixel);
    static Pixel convertCMYtoRGB(const Pixel& pixel);
    void readP5(char* fileName, int pos);
    void printP5(char* fileName, int pos);
public:
    explicit PPM (char* fileName);
    explicit PPM(char* fileName1, char* fileName2, char* fileName3);
    ~PPM();
    void print(char* fileName);
    void print(char* fileName1, char* fileName2, char* fileName3);
    void convert(char* from, char* to);
};



#endif //LAB4_PGM_H