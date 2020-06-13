#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "pgm.h"

PPM::PPM(char* fileName) {
    FILE* fin = fopen(fileName, "rb");
    if (fin == nullptr) {
        throw std::runtime_error("File can't be opened\n");
    }
    int tmp;
    if (fscanf(fin, "%s%d%d%d", header, &width, &height, &tmp) < 4) {
        throw std::runtime_error("Invalid header\n");
    }
    if (strcmp(header, "P6") != 0 &&  strcmp(header, "P5") != 0) {
        throw std::runtime_error("File is not a PGM image\n");
    }
    if (tmp > 255) {
        throw std::runtime_error("Unsupported colours\n");
    }
    maxValue = (uchar) tmp;
    if (width < 0 || height < 0) {
        throw std::runtime_error("Invalid header\n");
    }
    if (fgetc(fin) == EOF) {
        throw std::runtime_error("Invalid header\n");
    }
    data = new Pixel[width * height];
    if (strcmp(header, "P5") == 0) {
        uchar* dataP5 = new uchar[width * height];
        fread(dataP5, 1, width * height, fin);
        for (int i = 0; i < width * height; i++) {
            data[i] = Pixel(dataP5[i], dataP5[i], dataP5[i]);
        }
        delete[] dataP5;
    } else {
        fread(data, sizeof(Pixel), width * height, fin);
    }
    if (fclose(fin) != 0) {
        throw std::runtime_error("File can't be closed\n");
    }
}

void PPM::print(char *fileName) {
    FILE* fout = fopen(fileName, "wb");
    if (fout == nullptr) {
        throw std::runtime_error("File can't be opened or created\n");
    }
    fprintf(fout, "%s\n%i %i\n%i\n", header, width, height, maxValue);
    if (strcmp(header, "P5") == 0) {
        uchar* dataP5 = new uchar[width * height];
        for (int i = 0; i < width * height; i++) {
            dataP5[i] = data[i].first;
        }
        fwrite(dataP5, 1, width * height, fout);
        delete[] dataP5;
    } else {
        fwrite(data, sizeof(Pixel), width * height, fout);
    }
    fclose(fout);
}

PPM::~PPM() {
    delete[] data;
}

Pixel PPM::changeRGBPixel(const Pixel &pixel, const int& offset, const double& mult) {
    double r = std::max(std::min(((double)pixel.first - offset) * mult, 255.0), 0.0);
    double g = std::max(std::min(((double)pixel.second - offset) * mult, 255.0), 0.0);
    double b = std::max(std::min(((double)pixel.third - offset) * mult, 255.0), 0.0);
    return Pixel(r, g, b);
}

Pixel PPM::changeYCbCr_601Pixel(const Pixel &pixel, const int &offset, const double&mult) {
    double Y = std::max(std::min(((double)pixel.first - offset) * mult, 255.0), 0.0);
    return Pixel(Y, pixel.second, pixel.third);
}

Pixel PPM::convertYCbCr_601toRGB(const Pixel &pixel) {
    double Y = (double)pixel.first / 255.0;
    double Cb = (double)pixel.second / 255.0 - 0.5;
    double Cr = (double)pixel.third / 255.0 - 0.5;
    double R = Y + 1.402 * Cr;
    double G = Y - (0.299 * 1.402 / 0.587) * Cr - (0.114 * 1.772 / 0.587) * Cb;
    double B = Y + 1.772 * Cb;
    uchar r = std::max(std::min(255.0, 255 * R), 0.0);
    uchar g = std::max(std::min(255.0, 255 * G), 0.0);
    uchar b = std::max(std::min(255.0, 255 * B), 0.0);
    return Pixel(r, g, b);
}

Pixel PPM::convertRGBtoYCbCr_601(const Pixel &pixel) {
    double R = (double)pixel.first / 255.0;
    double G = (double)pixel.second / 255.0;
    double B = (double)pixel.third / 255.0;
    double Y = 0.299 * R + 0.587 * G + 0.114 * B;
    double Cb = (B - Y) / 1.772 + 0.5;
    double Cr = (R - Y) / 1.402 + 0.5;
    Y = std::max(std::min(Y, 1.0), 0.0);
    Cb = std::max(std::min(Cb, 1.0), 0.0);
    Cr = std::max(std::min(Cr, 1.0), 0.0);

    return Pixel(255 * Y, 255 * Cb, 255 * Cr);
}

void PPM::changeRGB(int offset, double mult) {
    for (int i = 0; i < width * height; i++) {
        data[i] = changeRGBPixel(data[i], offset, mult);
    }
}

void PPM::changeYCbCr_601(int offset, double mult) {
    for (int i = 0; i < width * height; i++) {
        data[i] = convertYCbCr_601toRGB(changeYCbCr_601Pixel(convertRGBtoYCbCr_601(data[i]),
                offset, mult));
    }
}

std::pair<int, double> PPM::autoRGB() {
    int minVal = 1000, maxVal = -1;
    for (int i = 0; i < width * height; i++) {
        minVal = std::min(minVal, (int)data[i].first);
        minVal = std::min(minVal, (int)data[i].second);
        minVal = std::min(minVal, (int)data[i].third);
        maxVal = std::max(maxVal, (int)data[i].first);
        maxVal = std::max(maxVal, (int)data[i].second);
        maxVal = std::max(maxVal, (int)data[i].third);
    }
    int offset = minVal;
    double mult = maxVal == minVal? 255.0 : 255.0  / (maxVal - minVal);
    for (int i = 0; i < width * height; i++) {
        data[i] = changeRGBPixel(data[i], offset, mult);
    }
    return {offset, mult};
}

std::pair<int, double> PPM::autoYCbCr_601() {
    for (int i = 0; i < width * height; i++) {
        data[i] = convertRGBtoYCbCr_601(data[i]);
    }
    int minVal = 1000, maxVal = -1;
    for (int i = 0; i < width * height; i++) {
        minVal = std::min(minVal, (int)data[i].first);
        maxVal = std::max(maxVal, (int)data[i].first);
    }
    int offset = minVal;
    double mult = maxVal == minVal? 255.0 : 255.0  / (maxVal - minVal);
    for (int i = 0; i < width * height; i++) {
        data[i] = convertYCbCr_601toRGB(changeYCbCr_601Pixel(data[i], offset, mult));
    }
    return {offset, mult};
}

