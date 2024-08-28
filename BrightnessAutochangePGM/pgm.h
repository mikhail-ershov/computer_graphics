#ifndef LAB5_PGM_H
#define LAB5_PGM_H

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
    static Pixel changeRGBPixel(const Pixel& pixel, const int& offset, const double& mult);
    static Pixel changeYCbCr_601Pixel(const Pixel& pixel, const int& offset, const double& mult);
    static Pixel convertRGBtoYCbCr_601(const Pixel& pixel);
    static Pixel convertYCbCr_601toRGB(const Pixel& pixel);
public:
    explicit PPM (char* fileName);
    ~PPM();
    void print(char* fileName);
    void changeRGB(int offset, double mult);
    void changeYCbCr_601(int offset, double mult);
    std::pair<int, double> autoRGB();
    std::pair<int, double> autoYCbCr_601();
};
#endif //LAB5_PGM_H
