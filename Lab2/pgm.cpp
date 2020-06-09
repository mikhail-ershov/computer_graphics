#include "pgm.h"
#include <algorithm>

PGM::PGM(char* fileName) {
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
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Invalid header\n");
    }
    if (fgetc(fin) == EOF) {
        throw std::runtime_error("Invalid header\n");
    }
    data = new uchar[width * height];
    fread(data, 1, width * height, fin);
    if (fclose(fin) != 0) {
        throw std::runtime_error("File can't be closed\n");
    }
}

PGM::~PGM() {
    delete[] data;
}
void PGM::print(char *fileName) {
    FILE* fout = fopen(fileName, "wb");
    if (fout == nullptr) {
        throw std::runtime_error("File can't be opened or created\n");
    }
    fprintf(fout, "%s\n%i %i\n%i\n", header, width, height, maxValue);
    fwrite(data, 1, width * height, fout);
    fclose(fout);
    uchar _max = 0;
    for (int i = 0; i < width * height; i++) {
        _max = std::max(_max, data[i]);
    }
}

void PGM::plot(Point point, double intensity, int brightness, double gamma) {
    if (point.x < 0 || point.x > width || point.y < 0 || point.y > height || brightness < 0) {
        return;
    }
    int index = int(std::round(point.y)) * width + int(std::round(point.x));
    double currentBrightness = (double)data[index] / 255;
    if (gamma == 0) {
        currentBrightness = currentBrightness <= 0.04045 ?
                            currentBrightness / 12.92 :
                            std::pow((currentBrightness + 0.055) / 1.055, 2.4);
    } else {
        currentBrightness = std::pow(currentBrightness, gamma);
    }
    currentBrightness *= (1.0 - intensity);
    double relativeBrightness = (double)brightness / 255.0;
    if (gamma == 0) {
        double decodedBrightness = relativeBrightness <= 0.04045 ?
                                   relativeBrightness / 12.92 :
                                   std::pow((relativeBrightness + 0.055) / 1.055, 2.4);
        currentBrightness += intensity * decodedBrightness;
        currentBrightness = currentBrightness <= 0.0031308?
                            currentBrightness * 12.92 :
                            std::pow(currentBrightness, 1.0 / 2.4) * 1.055 - 0.055;
    } else {
        double decodedBrightness = pow(relativeBrightness, gamma);
        currentBrightness += intensity * decodedBrightness;
        currentBrightness = std::pow(currentBrightness, 1.0 / gamma);
    }
    if (1.0 - currentBrightness < 1e-5) {
        currentBrightness = 1.0;
    }
    data[index] = 255 * currentBrightness;
}

bool insideRectangle(const Point& x, const Point& a, const Point& b, const Point& c, const Point& d) {
    Point ax = a - x;
    Point bx = b - x;
    Point cx = c - x;
    Point dx = d - x;
    double res[4] = {ax^bx, bx^cx, cx^dx, dx^ax};
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            if (res[i] * res[j] < 0) {
                return false;
            }
        }
    }
    return true;
}

double calculateIntensity(const Point& x, const Point& a, const Point& b, const Point& c, const Point& d, bool incline) {
    if (!incline) {
        if (insideRectangle(x, a, b, c, d)) {
            return 1.0;
        } else {
            return 0.0;
        }
    }
    int insideRect = 0;
    int total = 0;
    for (double i = -0.5; i <= 0.5; i+= 0.1) {
        for (double j = -0.5; j <= 0.5; j+= 0.1) {
            if (insideRectangle(Point(x.x + i, x.y + j), a, b, c, d)) {
                insideRect++;
            }
            total++;
        }
    }
    return (double)insideRect / total;
}

double calculateIntensity(const Point& x, const double& A, const double& B, const double& C) {
    double d = std::abs(A * x.x + B * x.y + C) / std::sqrt(A * A + B * B);
    if (d < 1e-2) {
        d = 0.0;
    }
    if (d > 0.9) {
        d = 1.0;
    }
    return 1.0 - d;
}

int min(double a, double b, double c, double d) {
    return round(std::min(std::min(a, b), std::min(c, d)));
}

int max(double a, double b, double c, double d) {
    return round(std::max(std::max(a, b), std::max(c, d)));
}

void PGM::drawLine(Point begin, Point end, int brightness, double thickness, double gamma) {
    if (begin.x < 0 || begin.x >= width || begin.y < 0 || begin.y >= height) {
        throw std::runtime_error("First point is out of bounds\n");
    }
    if (end.x < 0 || end.x >= width || end.y < 0 || end.y >= height) {
        throw std::runtime_error("Second point is out of bounds\n");
    }
    if (brightness < 0 || brightness > 255) {
        throw std::runtime_error("Invalid brightness\n");
    }
    if (thickness <= 0) {
        throw std::runtime_error("Invalid thickness\n");
    }
    Point bot1, bot2, top1, top2;
    Point vector;
    vector = {begin.y - end.y, end.x - begin.x};
    bool incline = (vector.x != 0 && vector.y != 0);
    vector = {vector.x / vector.length(), vector.y / vector.length()};
    double c = thickness / 2.0;
    if (c < 1e-5) {
        c = 0.0;
    }
    Point k = vector * c;
    bot1 = begin - k;
    bot2 = end - k;
    top1 = begin + k;
    top2 = end + k;
    for (int i = std::max(0, min(bot1.x, bot2.x, top1.x, top2.x)); i <= std::min(width - 1, max(bot1.x, bot2.x, top1.x, top2.x)); i++) {
        for (int j = std::max(0, min(bot1.y, bot2.y, top1.y, top2.y)); j <= std::min(height, max(bot1.y, bot2.y, top1.y, top2.y)); j++) {
            Point x(i, j);
            if (!incline || thickness > 1.0) {
                plot(x, calculateIntensity(x, bot1, top1, top2, bot2, incline), brightness, gamma);
            } else {
                plot(x, calculateIntensity(x, begin.y - end.y, end.x - begin.x, begin^end), brightness, gamma);
            }
        }
    }
}