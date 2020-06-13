#include <iostream>
#include <string>
#include <cstdlib>
#include "ppm.h"

using namespace std;

int main(int argc, char* argv[]) {
    char *from, *to;
    int count_in, count_out;
    char *input;
    char* output;
    try {
        for (int i = 0; i < argc; i++) {
            if (string(argv[i]) == "-f") {
                if (i != argc - 1) {
                    from = argv[i + 1];
                } else {
                    cerr << "Invalid input\n";
                    return 1;
                }
            }
            if (string(argv[i]) == "-t") {
                if (i != argc - 1) {
                    to = argv[i + 1];
                } else {
                    cerr << "Invalid input\n";
                    return 1;
                }
            }
            if (string(argv[i]) == "-i" && i != argc - 1) {
                count_in = atoi(argv[i + 1]);
                if (i < argc - 2) {
                    input = argv[i + 2];
                } else {
                    cerr << "Invalid input\n";
                    return 1;
                }
            }
            if (string(argv[i]) == "-o" && i != argc - 1) {
                count_out = atoi(argv[i + 1]);
                if (i < argc - 2) {
                    output = argv[i + 2];
                } else {
                    cerr << "Invalid input\n";
                    return 1;
                }
            }
        }
        if (count_in != 1 && count_in != 3 || count_out != 1 && count_out != 3) {
            cerr << "Invalid input\n";
            return 1;
        }
    } catch (const exception& e) {
        cerr << "Invalid input\n" << e.what() << "\n";
        return 1;
    }
    try{
        if (count_in == 1) {
            PPM image(input);
            image.convert(from, to);
            if (count_out == 1) {
                image.print(output);
            } else {
                image.print(output, 3);
            }
        } else {
            PPM image(input, 3);
            image.convert(from, to);
            if (count_out == 1) {
                image.print(output);
            } else {
                image.print(output, 3);
            }
        }
    } catch(const exception& e) {
        cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
