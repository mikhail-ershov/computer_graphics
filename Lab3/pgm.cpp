#include "pgm.h"
#include <stdexcept>
#include <set>
#include <iostream>
#include <cmath>

PGM::PGM(char* fileName, bool gradient, double gamma) {
    FILE* fin = fopen(fileName, "rb");
    if (fin == nullptr) {
        throw std::runtime_error("File can't be opened\n");
    }
    int tmp;
    if (fscanf(fin, "%s%d%d%d", header, &width, &height, &tmp) < 4) {
        throw std::runtime_error("Invalid header\n");
    }
    if (strcmp(header, "P5") != 0) {
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
    data = new uchar[width * height];
    errors = new double[width * height];
    for (int i = 0; i < width * height; i++) {
        errors[i] = 0;
    }
    if (!gradient) {
        fread(data, 1, width * height, fin);
        for (int i = 0; i < width * height; i++) {
            double relativeBrightness = (double) data[i] / maxValue;
            if (gamma == 0) {
                relativeBrightness = relativeBrightness <= 0.04045?
                        relativeBrightness / 12.92 :
                        std::pow((relativeBrightness + 0.055) / 1.055, 2.4);
            } else {
                relativeBrightness = std::pow(relativeBrightness, gamma);
            }
            data[i] = maxValue * relativeBrightness;
        }
    } else {
        for (int i = 0; i < height * width; i++) {
            data[i] = (i % width) * 256 / width;
        }
    }
    if (fclose(fin) != 0) {
        throw std::runtime_error("File can't be closed\n");
    }
}

PGM::~PGM() {
    delete[] data;
    delete[] errors;
}

void PGM::print(char *fileName, int bit, bool gradient, double gamma) {
    FILE* fout = fopen(fileName, "wb");
    if (fout == nullptr) {
        throw std::runtime_error("File can't be opened or created\n");
    }
    if (!gradient) {
        for (int i = 0; i < width * height; i++) {
            double relativeBrightness = (double) data[i] / maxValue;
            if (gamma == 0) {
                relativeBrightness = relativeBrightness <= 0.0031308 ?
                                     relativeBrightness * 12.92 :
                                     std::pow(1.055 * relativeBrightness, 1 / 2.4) - 0.055;
            } else {
                relativeBrightness = std::pow(relativeBrightness, 1 / gamma);
            }
            data[i] = maxValue * relativeBrightness;
        }
    }
    fprintf(fout, "%s\n%i %i\n%i\n", header, width, height, maxValue);
    fwrite(data, 1, width * height, fout);
    fclose(fout);
}

int BayerMatrix[8][8] = {
        {0, 48, 12, 60, 3, 51, 15, 63},
        {32, 16, 44, 28, 35, 19, 47, 31},
        {8, 56, 4, 52, 11, 59, 7, 55},
        {40, 24, 36, 20, 43, 27, 39, 23},
        {2, 50, 14, 62, 1, 49, 13, 61},
        {34, 18, 46, 30, 33, 17, 45, 29},
        {10, 58, 6, 54, 9, 57, 5, 53},
        {42, 26, 38, 22, 41, 25, 37, 21}
        };
int HalftoneMatrix[4][4] = {
        {7, 13, 11, 4},
        {12, 16, 14, 8},
        {10, 15, 6, 2},
        {5, 9, 3, 1}
        };

uchar nearestColor(const uchar& color, const unsigned int& bit) {
    uchar borderColor = ((1u << bit) - 1);
    borderColor &= color >> (8u - bit);
    uchar additionalColor = borderColor >> (bit - (8 % bit));;
    uchar newColor = 0;
    for (int i = 0; i < 8 / bit; i++) {
        newColor += borderColor << i * bit;
    }
    return (newColor << (8 % bit)) + additionalColor;
}

double sum(double a, double b) {
    a += b;
    if(a <= 0.0)
        return 0.0;
    if(a >= 1.0)
        return 1.0;
    return a;
}

void PGM::dither(int bit, int typeOfDithering, double gamma) {
    switch(typeOfDithering) {
        case 0:
            for (int i = 0; i < width * height; i++) {
                data[i] = nearestColor(data[i], bit);
            }
            break;
        case 1:
            for (int i = 0; i < width * height; i++) {
                int row = i / width;
                int col = i % width;
                double tmp = (double)BayerMatrix[row % 8][col % 8] / 64 - 0.5;
                double color = (double)data[i] / 255.0;
                data[i] = nearestColor(sum(color, tmp) * 255, bit);
            }
            break;
        case 2:
            for (int i = 0; i < width * height; i++) {
                double tmp = (double) rand() / (RAND_MAX - 1.) - 0.5;
                double color = (double)data[i] / 255.0;
                data[i] = nearestColor(sum(color, tmp) * 255, bit);
            }
            break;
        case 3:
            for (int i = 0; i < width * height; i++) {
                double color = (double)data[i] / 255.;
                color = sum(color, errors[i]);
                data[i] = nearestColor(color * 255, bit);
                double newColor = (double) data[i] / 255.0;
                double error = (color - newColor) / 16.0;
                int row = i / width;
                int col = i % width;
                if (col + 1 < width) {
                    if (row + 1 < height) {
                        errors[(row + 1) * width + col + 1] += error * 1.0;
                    }
                    errors[row * width + col + 1] += error * 7.0;
                }
                if (row + 1 < height) {
                    if (col - 1 >= 0) {
                        errors[(row + 1) * width + col - 1] += error * 3.0;
                    }
                    errors[(row + 1) * width + col] += error * 5.0;
                }
            }
            break;
        case 4:
            for (int i = 0; i < width * height; i++) {
                double color = (double) data[i] / 255.;
                color = sum(color, errors[i]);
                data[i] = nearestColor(color * 255, bit);
                double newColor = (double) data[i] / 255.0;
                double error = (color - newColor) / 48.0;
                int row = i / width;
                int col = i % width;
                if (row + 1 < height) {
                    //row + 1, col + 0
                    if (row + 2 < height) {
                        // row + 2, col + 0
                        errors[(row + 2) * width + col] += error * 5.0;
                    }
                    errors[(row + 1) * width + col] += error * 7.0;
                }
                if (col + 1 < width) {
                    // row + 0, col + 1
                    if (col + 2 < width) {
                        //row + 0, col + 2
                        if (row + 1 < height) {
                            // row + 1, col + 2
                            if (row + 2 < height) {
                                // row + 2, col + 2
                                errors[(row + 2) * width + col + 2] += error * 1.0;
                            }
                            errors[(row + 1) * width + col + 2] += error * 3.0;
                        }
                        errors[row * width + col + 2] += error * 5.0;
                    }
                    if (row + 1 < height) {
                        // row + 1, col + 1
                        if (row + 2 < height) {
                            // row + 2, col + 1
                            errors[(row + 2) * width + col + 1] += error * 3.0;
                        }
                        errors[(row + 1) * width + col + 1] += error * 5.0;
                    }
                    errors[row * width + col + 1] += error * 7.0;
                }
                if (col - 1 >= 0) {
                    //row + 0, col - 1
                    if (col - 2 >= 0) {
                        // row + 0, col - 2
                        if (row + 1 < height) {
                            // row + 1, col - 2
                            if (row + 2 < height) {
                                // row + 2, col - 2
                                errors[(row + 2) * width + col - 2] += error * 1.0;
                            }
                            errors[(row + 1) * width + col - 2] += error * 3.0;
                        }
                    }
                    if (row + 1 < height) {
                        // row + 1, col - 1
                        if (row + 2 < height) {
                            // row + 2, col - 1
                            errors[(row + 2) * width + col - 1] += error * 3.0;
                        }
                        errors[(row + 1) * width + col - 1] += error * 5.0;
                    }
                }
            }
            break;
        case 5:
            for (int i = 0; i < width * height; i++) {
                double color = (double) data[i] / 255.;
                color = sum(color, errors[i]);
                data[i] = nearestColor(color * 255, bit);
                double newColor = (double) data[i] / 255.0;
                double error = (color - newColor) / 32.0;
                int row = i / width;
                int col = i % width;
                if (row + 1 < height) {
                    if (row + 2 < height) {
                        errors[(row + 2) * width + col] += error * 3.0;
                    }
                    errors[(row + 1) * width + col] += error * 5.0;
                }
                if (col + 1 < width) {
                    if (col + 2 < width) {
                        if (row + 1 < height) {
                            errors[(row + 1) * width + col + 2] += error * 2.0;
                        }
                        errors[row * width + col + 2] += error * 3.0;
                    }
                    if (row + 1 < height) {
                        if (row + 2 < height) {
                            errors[(row + 2) * width + col + 1] += error * 2.0;
                        }
                        errors[(row + 1) * width + col + 1] += error * 4.0;
                    }
                    errors[row * width + col + 1] += error * 5.0;
                }
                if (col - 1 >= 0) {
                    if (col - 2 >= 0) {
                        if (row + 1 < height) {
                            errors[(row + 1) * width + col - 2] += error * 2.0;
                        }
                    }
                    if (row + 1 < height) {
                        if (row + 2 < height) {
                            errors[(row + 2) * width + col - 1] += error * 2.0;
                        }
                        errors[(row + 1) * width + col - 1] += error * 4.0;
                    }
                }
            }
            break;
        case 6:
            for (int i = 0; i < width * height; i++) {
                double color = (double) data[i] / 255.;
                color = sum(color, errors[i]);
                data[i] = nearestColor(color * 255, bit);
                double newColor = (double) data[i] / 255.0;
                double error = (color - newColor) / 8.0;
                int row = i / width;
                int col = i % width;
                if (row + 1 < height) {
                    if (row + 2 < height) {
                        errors[(row + 2) * width + col] += error;
                    }
                    errors[(row + 1) * width + col] += error;
                }
                if (col + 1 < width) {
                    if (col + 2 < width) {
                        errors[row * width + col + 2] += error;
                    }
                    if (row + 1 < height) {
                        errors[(row + 1) * width + col + 1] += error;
                    }
                    errors[row * width + col + 1] += error;
                }
                if (col - 1 >= 0) {
                    if (row + 1 < height) {
                        errors[(row + 1) * width + col - 1] += error;
                    }
                }
            }
            break;
        case 7:
            for (int i = 0; i < height * width; i++) {
                int row = i / width;
                int col = i % width;
                double tmp = (double) 1.0 * HalftoneMatrix[row % 4][col % 4] / 17.0 - 0.5;
                double color = (double)data[i] / 255.0;
                data[i] = nearestColor(sum(color, tmp) * 255, bit);
            }
            break;
        default: throw std::runtime_error("Invalid type of dithering!\n");
    }
}
