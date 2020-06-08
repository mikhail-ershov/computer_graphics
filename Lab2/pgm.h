#ifndef LAB2_PGM_H
#define LAB2_PGM_H

#include <cstdio>
#include <cstring>
#include <cmath>
#include <stdexcept>

typedef unsigned char uchar;

struct Point{
    double x, y;
    Point(double x, double y) {
        this->x = x;
        this->y = y;
    }
    Point() = default;
    double length() const {
        return std::sqrt(x * x + y * y);
    }
    Point operator-(const Point& point) const {
        return {this->x - point.x, this->y - point.y};
    }

    Point operator+(const Point& point) const {
        return {this->x + point.x, this->y + point.y};
    }

    Point operator*(const double& k) const{
        return {this->x * k, this->y * k};
    }

    double operator^(const Point& point) const{
        return x * point.y - y * point.x;
    }
};

class PGM{
private:
    char header[2];
    int width, height;
    uchar maxValue;
    uchar* data;
    void plot(Point point, double brightness, double gamma = 0);
public:
    explicit PGM (char* fileName);
    ~PGM();
    void print(char* fileName);
    void drawLine(Point begin, Point end, double brightness, double thickness, double gamma = 0);
};

#endif //LAB2_PGM_H
