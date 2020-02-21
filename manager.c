#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ppm.h"
#include "pgm.h"

void manageFile(char* inputFileName, char* outputFileName, int action) {
    FILE* fin = fopen(inputFileName, "rb");
    if (fin == NULL) {
        printf("File can't be opened\n");
        return;
    }
    char header[3];
    fread(header, 1, 2, fin);
    header[2] = '\0';
    fclose(fin);
    if (strcmp(header, "P5") == 0) {
        PGM* pgm = readPGM(inputFileName);
        switch(action){
            case 0: inversePGM(pgm); writePGM(outputFileName, pgm);  break;
            case 1: reflectPGM_H(pgm); writePGM(outputFileName, pgm); break;
            case 2: reflectPGM_V(pgm); writePGM(outputFileName, pgm); break;
            case 3: rotatePGM_R(pgm); writePGM(outputFileName, pgm); break;
            case 4: rotatePGM_L(pgm); writePGM(outputFileName, pgm); break;
            default: printf("Incorrect action\n"); break;
        }
        if (pgm != NULL) {
            free(pgm->data);
            free(pgm);
        }
        return;
    }
    if (strcmp(header, "P6") == 0) {
        PPM* ppm = readPPM(inputFileName);
        switch(action) {
            case 0: inversePPM(ppm); writePPM(outputFileName, ppm); break;
            case 1: reflectPPM_H(ppm); writePPM(outputFileName, ppm); break;
            case 2: reflectPPM_V(ppm); writePPM(outputFileName, ppm); break;
            case 3: rotatePPM_R(ppm); writePPM(outputFileName, ppm); break;
            case 4: rotatePPM_L(ppm); writePPM(outputFileName, ppm); break;
            default: printf("Incorrect action\n"); break;
        }
        if (ppm != NULL) {
            free(ppm->data);
            free(ppm);
        }
        return;
    }
    printf("File is not a pbm file\n");
}