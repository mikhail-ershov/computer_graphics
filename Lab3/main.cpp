#include <iostream>
#include <string>
#include <cstdlib>
#include "pgm.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 7) {
        cerr << "Invalid number of arguments\n";
        return 1;
    }
    try{
        bool gradient = atoi(argv[3]);
        int typeOfDithering = atoi(argv[4]);
        int bit = atoi(argv[5]);
        double gamma = stod(argv[6]);
        PGM image(argv[1], gradient, gamma);
        image.dither(bit, typeOfDithering, gamma);
        image.print(argv[2], bit, gamma);
    } catch (const exception& e) {
        std::cerr << e.what();
        return 1;
    }

    return 0;
}
