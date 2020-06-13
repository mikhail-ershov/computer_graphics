#include "ppm.h"
#include <stdexcept>
#include <string>
#include <algorithm>

PPM::PPM(char* fileName) {
    FILE* fin = fopen(fileName, "rb");
    if (fin == nullptr) {
        throw std::runtime_error("File can't be opened\n");
    }
    int tmp;
    if (fscanf(fin, "%s%d%d%d", header, &width, &height, &tmp) < 4) {
        throw std::runtime_error("Invalid header\n");
    }
    if (strcmp(header, "P6") != 0) {
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
    fread(data, sizeof(Pixel), width * height, fin);
    if (fclose(fin) != 0) {
        throw std::runtime_error("File can't be closed\n");
    }
}

void PPM::readP5(char* fileName, int pos) {
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
    if (data == NULL) {
        data = new Pixel[width * height];
    }
    uchar* dataP5 = new uchar[width * height];
    fread(dataP5, sizeof(uchar), width * height, fin);
    for (int i = 0; i < width * height; i++) {
        switch(pos) {
            case 0: data[i].first  = dataP5[i]; break;
            case 1: data[i].second = dataP5[i]; break;
            case 2: data[i].third = dataP5[i]; break;
        }
    }
    delete[] dataP5;
    if (fclose(fin) != 0) {
        throw std::runtime_error("File can't be closed\n");
    }
}

PPM::PPM(char *fileName, int num) {
    data = NULL;
    std::string in(fileName);
    int pos = -1;
    for (int i = in.size() - 1; i >= 0; i--) {
        if (in[i] == '.') {
            pos = i;
            break;
        }
    }
    readP5((char*)std::string(in.substr(0, pos) + "_1" + in.substr(pos, in.size())).c_str(), 0);
    readP5((char*)std::string(in.substr(0, pos) + "_2" + in.substr(pos, in.size())).c_str(), 1);
    readP5((char*)std::string(in.substr(0, pos) + "_3" + in.substr(pos, in.size())).c_str(), 2);
    strcpy(header, "P6");
}

PPM::~PPM() {
    delete[] data;
}

void PPM::print(char *fileName) {
    FILE* fout = fopen(fileName, "wb");
    if (fout == nullptr) {
        throw std::runtime_error("File can't be opened or created\n");
    }
    fprintf(fout, "%s\n%i %i\n%i\n", header, width, height, maxValue);
    fwrite(data, sizeof(Pixel), width * height, fout);
    fclose(fout);
}

void PPM::printP5(char *fileName, int pos) {
    FILE* fout = fopen(fileName, "wb");
    if (fout == nullptr) {
        throw std::runtime_error("File can't be opened or created\n");
    }
    uchar* dataP5 = new uchar[width * height];
    for (int i = 0; i < width * height; i++) {
        switch(pos) {
            case 0: dataP5[i] = data[i].first; break;
            case 1: dataP5[i] = data[i].second; break;
            case 2: dataP5[i] = data[i].third; break;
        }
    }
    fprintf(fout, "%s\n%i %i\n%i\n", "P5", width, height, maxValue);
    fwrite(dataP5, 1, width * height, fout);
    fclose(fout);
}

void PPM::print(char *fileName, int num) {
    std::string in(fileName);
    int pos = -1;
    for (int i = in.size() - 1; i >= 0; i--) {
        if (in[i] == '.') {
            pos = i;
            break;
        }
    }
    printP5((char*)std::string(in.substr(0, pos) + "_1" + in.substr(pos, in.size())).c_str(), 0);
    printP5((char*)std::string(in.substr(0, pos) + "_2" + in.substr(pos, in.size())).c_str(), 1);
    printP5((char*)std::string(in.substr(0, pos) + "_3" + in.substr(pos, in.size())).c_str(), 2);
}

void PPM::convert(char *from, char *to) {
    std::string f = std::string(from);
    std::string t = std::string(to);
    if (f == "HSL" && t == "HSV") {
        for (int i = 0; i < height * width; i++) {
            data[i] = convertHSLtoHSV(data[i]);
        }
        return;
    }
    if (f == "HSV" && t == "HSL") {
        for (int i = 0; i < height * width; i++) {
            data[i] = convertHSVtoHSL(data[i]);
        }
        return;
    }

    for (int i = 0; i < height * width; i++) {
        if (f == "HSV") {
            data[i] = convertHSLtoRGB(convertHSVtoHSL(data[i]));
        }
        if (f == "HSL") {
            data[i] = convertHSLtoRGB(data[i]);
        }
        if (f == "CMY") {
            data[i] = convertCMYtoRGB(data[i]);
        }
        if (f == "YCbCr.601") {
            data[i] = convertYCbCr_601toRGB(data[i]);
        }
        if (f == "YCbCr.709") {
            data[i] = convertYCbCr_709toRGB(data[i]);
        }
        if (f == "YCoCg") {
            data[i] = convertYCoCgtoRGB(data[i]);
        }
    }

    for (int i = 0; i < height * width; i++) {
        if (t == "HSV") {
            data[i] = convertHSLtoHSV(convertRGBtoHSL(data[i]));
        }
        if (t == "HSL") {
            data[i] = convertRGBtoHSL(data[i]);
        }
        if (t == "CMY") {
            data[i] = convertRGBtoCMY(data[i]);
        }
        if (t == "YCbCr.601") {
            data[i] = convertRGBtoYCbCr_601(data[i]);
        }
        if (t == "YCbCr.709") {
            data[i] = convertRGBtoYCbCr_709(data[i]);
        }
        if (t == "YCoCg") {
            data[i] = convertRGBtoYCoCg(data[i]);
        }
    }
}

Pixel PPM::convertRGBtoHSL(const Pixel& pixel) {
    double R = (double)pixel.first / 255.0;
    double G = (double)pixel.second / 255.0;
    double B = (double)pixel.third / 255.0;
    double Cmax = std::max(std::max(R, G), B);
    double Cmin = std::min(std::min(R, G), B);
    double delta = Cmax - Cmin;
    if (delta < 1e-5) {
        delta = 0.0;
    }
    double L = (Cmax + Cmin) / 2.0;
    double H, S;
    if (delta == 0) {
        H = 0.0;
        S = 0.0;
    } else {
        S = delta / (1 - std::abs(2.0 * L - 1.0));
        if (Cmax == R) {
            H = (G - B) / delta;
        }
        if (Cmax == G) {
            H = (B - R) / delta + 2.0;
        }
        if (Cmax == B) {
            H = (R - G) / delta + 4;
        }
        H *= 60.0;
    }
    return Pixel((H / 360.0) * 255, S * 255, L * 255);
}

Pixel PPM::convertHSLtoHSV(const Pixel &pixel) {
    double S = (double)pixel.second / 255.0;
    double L = (double)pixel.third / 255.0;
    S *= L < 0.5? L : 1 - L;
    double newS = 2 * S / (L + S);
    double newV = L + S;
    return Pixel(pixel.first, 255 * newS, 255 * newV);
}

Pixel PPM::convertHSVtoHSL(const Pixel &pixel) {
    double S = (double)pixel.second / 255.0;
    double V = (double)pixel.third / 255.0;
    double L = (2.0 - S) * V / 2.0;
    if (L < 1e-5) {
        L = 0.0;
    }
    if (1 - L < 1e-5) {
        L = 1.0;
    }
    if (L != 0.0) {
        if (L == 1.0) {
            S = 0.0;
        } else if (L < 0.5) {
            S = S * V / (L * 2.0);
        } else {
            S = S * V / (2.0 - L * 2.0);
        }
    }

    return Pixel(pixel.first, 255 * S, 255 * L);
};

double HtoRGB(double p, double q, double t) {
    if (t < 0.0) {
        t += 1.0;
    }
    if (t > 1.0) {
        t -= 1.0;
    }
    if (t < 1.0/6.0) {
        return p + (q - p) * 6.0 * t;
    }
    if (t < 0.5) {
        return q;
    }
    if (t < 2.0/3.0) {
        return p + (q - p) * (2.0/3.0 - t) * 6.0;
    }
    return p;
}

Pixel PPM::convertHSLtoRGB(const Pixel &pixel) {
    double H = (double)pixel.first / 255.0;
    double S = (double)pixel.second / 255.0;
    double L = (double)pixel.third / 255.0;
    double R, G, B;
    if (S < 1e-5) {
        R = G = B = L;
    } else {
        double q = L < 0.5? L * (1.0 + S) : L + S - L * S;
        double p = 2.0 * L - q;
        R = HtoRGB(p, q, H + 1.0/3.0);
        G = HtoRGB(p, q, H);
        B = HtoRGB(p, q, H - 1.0/3.0);
    }
    return Pixel(R * 255, G * 255, B * 255);
}

Pixel PPM::convertCMYtoRGB(const Pixel &pixel) {
    return Pixel(255  - pixel.first, 255 - pixel.second, 255 - pixel.third);
}

Pixel PPM::convertRGBtoCMY(const Pixel &pixel) {
    return Pixel(255  - pixel.first, 255 - pixel.second, 255 - pixel.third);
}

Pixel PPM::convertRGBtoYCbCr_601(const Pixel &pixel) {
    double R = (double)pixel.first / 255.0;
    double G = (double)pixel.second / 255.0;
    double B = (double)pixel.third / 255.0;
    double Y = 0.299 * R + 0.587 * G + 0.114 * B;
    double Cb = (B - Y) / 1.772 + 0.5;
    double Cr = (R - Y) / 1.402 + 0.5;
    return Pixel(255 * Y, 255 * Cb, 255 * Cr);
}

Pixel PPM::convertYCbCr_601toRGB(const Pixel &pixel) {
    double Y = (double)pixel.first / 255.0;
    double Cb = (double)pixel.second / 255.0 - 0.5;
    double Cr = (double)pixel.third / 255.0 - 0.5;
    double R = Y + 1.402 * Cr;
    double G = Y - (0.299 * 1.402 / 0.587) * Cr - (0.114 * 1.772 / 0.587) * Cb;
    double B = Y + 1.772 * Cb;
    return Pixel(R * 255, G * 255, B * 255);
}

Pixel PPM::convertRGBtoYCbCr_709(const Pixel &pixel) {
    double R = (double)pixel.first / 255.0;
    double G = (double)pixel.second / 255.0;
    double B = (double)pixel.third / 255.0;
    double Y = 0.2126 * R + 0.7152 * G + 0.0722 * B;
    double Cb = (B - Y) / 1.8556 + 0.5;
    double Cr = (R - Y) / 1.5748 + 0.5;
    return Pixel(Y * 255, Cb * 255, Cr * 255);
}

Pixel PPM::convertYCbCr_709toRGB(const Pixel &pixel) {
    double Y = (double)pixel.first / 255.0;
    double Cb = (double)pixel.second / 255.0 - 0.5;
    double Cr = (double)pixel.third / 255.0 - 0.5;
    double R = Y + 1.5748 * Cr;
    double G = Y - (0.2126 * 1.5748 / 0.7152) * Cr - (0.0722 * 1.8556 / 0.7152) * Cb;
    double B = Y + 1.8556 * Cb;
    return Pixel(R * 255, G * 255, B * 255);
}

Pixel PPM::convertRGBtoYCoCg(const Pixel &pixel) {
    double R = (double)pixel.first / 255.0;
    double G = (double)pixel.second / 255.0;
    double B = (double)pixel.third / 255.0;
    double Y = 0.25 * R + 0.5 * G + 0.25 * B;
    double Co = 0.5 * R - 0.5 * B + 0.5;
    double Cg = -0.25 * R + 0.5 * G - 0.25 * B + 0.5;
    return Pixel(Y * 255, Co * 255, Cg * 255);
}

Pixel PPM::convertYCoCgtoRGB(const Pixel &pixel) {
    double Y = (double)pixel.first / 255.0;
    double Co = (double)pixel.second / 255.0 - 0.5;
    double Cg = (double)pixel.third / 255.0 - 0.5;
    double R = Y + Co - Cg;
    double G = Y + Cg;
    double B = Y - Co - Cg;
    return Pixel(255 * R, 255 * G, 255 * B);
}

