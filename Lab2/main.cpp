#include <iostream>
#include "pgm.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 9 || argc > 10) {
        cerr << "Invalid number of arguments\n";
        return 1;
    }
    try {
        PGM image(argv[1]);
        if (argc == 9) {
            image.drawLine({stod(argv[5]), stod(argv[6])},
                        {stod(argv[7]), stod(argv[8])},
                        atoi(argv[3]),
                           stod(argv[4]));
        } else {
            image.drawLine({stod(argv[5]), stod(argv[6])},
                           {stod(argv[7]), stod(argv[8])},
                           atoi(argv[3]),
                           stod(argv[4]),
                           stod(argv[9]));
        }
        image.print(argv[2]);
    } catch (const exception& e) {
        cerr << e.what() << " what?\n";
        return 1;
    }
    return 0;
}
