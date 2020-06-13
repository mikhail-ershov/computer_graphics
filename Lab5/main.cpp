#include <iostream>
#include <string>
#include <cstdlib>
#include "pgm.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4 && argc != 6) {
        cerr << "Invalid number of arguments\n";
        return 1;
    }
    try {
        PPM image(argv[1]);
        int it = atoi(argv[3]);
        pair<int, double> res;
        switch (it) {
            case 0:
                image.changeRGB(atoi(argv[4]), stod(argv[5]));
                break;
            case 1:
                image.changeYCbCr_601(atoi(argv[4]), stod(argv[5]));
                break;
            case 2:
                res = image.autoRGB();
                cout << res.first << " " << res.second << "\n";
                break;
            case 3:
                res = image.autoYCbCr_601();
                cout << res.first << " " << res.second << "\n";
                break;
            default:
                cerr << "Invalid transformation\n";
                return 1;
        }
        image.print(argv[2]);
    } catch (const exception& e) {
        cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
