#include "pgm.h"

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
    if (width < 0 || height < 0) {
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
}

void PGM::plot(Point point, double brightness, double gamma) {
    if (point.x < 0 || point.x > width || point.y < 0 || point.y > height || brightness < 0) {
        return;
    }
    int index = std::round(point.y * width + point.x);
    double relativeBrightness = brightness / 255.0;
    if (gamma == 0) {
        double encodedBrightness = relativeBrightness <= 0.0031308 ?
                relativeBrightness * 12.92 :
                1.055 * std::pow(relativeBrightness, 1.0 / 2.4) - 0.055;
        data[index] = 255 * encodedBrightness;
    } else {
        data[index] = 255 * pow(relativeBrightness, 1 / gamma);
    }
}

bool insideRectangle(const Point& x, const Point& a, const Point& b, const Point& c, const Point& d) {
    Point ax = a - x;
    Point bx = b - x;
    Point cx = c - x;
    Point dx = d - x;
    double ab = ax^bx;
    double bc = bx^cx;
    double cd = cx^dx;
    double da = dx^ax;
    return ab * bc >= 0 && ab * cd >= 0 && ab * da >= 0;
}

int min(double a, double b, double c, double d) {
    return round(std::min(std::min(a, b), std::min(c, d)));
}

int max(double a, double b, double c, double d) {
    return round(std::max(std::max(a, b), std::max(c, d)));
}

void PGM::drawLine(Point begin, Point end, double brightness, double thickness, double gamma) {
    Point bot1, bot2, top1, top2;
    Point vector;
    vector = {begin.y - end.y, end.x - begin.x};
    vector = {vector.x / vector.length(), vector.y / vector.length()};
    Point k = vector * (thickness / 2);
    bot1 = begin - k;
    bot2 = end - k;
    top1 = begin + k;
    top2 = end + k;
    for (int i = min(bot1.x, bot2.x, top1.x, top2.x); i <= max(bot1.x, bot2.x, top1.x, top2.x); i++) {
        for (int j = min(bot1.y, bot2.y, top1.y, top2.y); j <= max(bot1.y, bot2.y, top1.y, top2.y); j++) {
            Point x(i, j);
            if (insideRectangle(x, bot1, top1, top2, bot2)) {
                plot(x, brightness, gamma);
            }
        }
    }
}