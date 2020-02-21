#include <stdio.h>
#include <stdlib.h>
#include "ppm.h"

PPM* readPPM(char* fileName) {
    FILE* fin = fopen(fileName, "rb");
    if (fin == NULL) {
        printf("File can't be opened\n");
        return NULL;
    }
    PPM* ppm = (PPM*) malloc(sizeof(PPM));
    if (ppm == NULL) {
        printf("Malloc error happened\n");
        return NULL;
    }
    int tmp;
    if (fscanf(fin, "%s%d%d%d", ppm->header, &(ppm->width), &(ppm->height), &tmp) < 4) {
        printf("Header reading error happened\n");
        free(ppm);
        return NULL;
    }
    if (tmp > 255) {
        printf("Unsupported colours\n");
        free(ppm);
        return NULL;
    }
    ppm->maxValue = (uchar) tmp;
    if (ppm->width < 0 || ppm->height < 0) {
        printf("Invalid header\n");
        free(ppm);
        return NULL;
    }
    if (fgetc(fin) == EOF) {
        printf("Data reading error happened\n");
        free(ppm);
        return NULL;
    }
    ppm->data = (PIXEL*) malloc(sizeof(PIXEL) * ppm->height * ppm->width);
    if (ppm->data == NULL) {
        printf("Malloc error happened\n");
        free(ppm);
        return NULL;
    }
    if (fread(ppm->data, sizeof(PIXEL), ppm->height * ppm->width, fin) < ppm->height * ppm->width) {
        printf("Data reading error happened\n");
        free(ppm->data);
        free(ppm);
        return NULL;
    }
    if (fclose(fin) != 0) {
        printf("File can't be close\n");
    }
    return ppm;
}

void inversePPM(PPM* ppm) {
    if (ppm == NULL) {
        return;
    }
    for (int i = 0; i < ppm->height * ppm->width; i++) {
            ppm->data[i].r = ppm->maxValue - ppm->data[i].r;
            ppm->data[i].g = ppm->maxValue - ppm->data[i].g;
            ppm->data[i].b = ppm->maxValue - ppm->data[i].b;
    }
}

void reflectPPM_V(PPM* ppm) {
    if (ppm == NULL) {
        return;
    }
    for (int i = 0; i < ppm->height / 2; i++) {
        for (int j = 0; j < ppm->width; j++) {
            PIXEL tmp = ppm->data[i * ppm->width + j];
            ppm->data[i * ppm->width + j] = ppm->data[(ppm->height - i - 1) * ppm->width + j];
            ppm->data[(ppm->height - i - 1) * ppm->width + j] = tmp;
        }
    }
}

void reflectPPM_H(PPM* ppm) {
    if (ppm == NULL) {
        return;
    }
    for (int i = 0; i < ppm->height; i++) {
        for (int j = 0; j < ppm->width / 2; j++) {
            PIXEL tmp = ppm->data[i * ppm->width + j];
            ppm->data[i * ppm->width + j] = ppm->data[i * ppm->width + ppm->width - j - 1];
            ppm->data[i * ppm->width + ppm->width - j - 1] = tmp;
        }
    }
}

void rotatePPM_L(PPM* ppm) {
    if (ppm == NULL) {
        return;
    }
    int tmp = ppm->height;
    ppm->height = ppm->width;
    ppm->width = tmp;
    PIXEL* buffer = (PIXEL*) malloc(sizeof(PIXEL) * ppm->width * ppm->height);
    if (buffer == NULL) {
        printf("Malloc error happened\n");
        free(ppm->data);
        free(ppm);
        ppm = NULL;
        return;
    }
    for (int i = 0; i < ppm->height; i++) {
        for (int j = 0; j < ppm->width; j++) {
            buffer[i * ppm->width + j] = ppm->data[j * ppm->height + ppm->height - i - 1];
        }
    }
    free(ppm->data);
    ppm->data = buffer;
}

void rotatePPM_R(PPM* ppm) {
    if (ppm == NULL) {
        return;
    }
    int tmp = ppm->height;
    ppm->height = ppm->width;
    ppm->width = tmp;
    PIXEL* buffer = (PIXEL*) malloc(sizeof(PIXEL) * ppm->width * ppm->height);
    if (buffer == NULL) {
        printf("Malloc error happened\n");
        free(ppm->data);
        free(ppm);
        ppm = NULL;
        return;
    }
    for (int i = 0; i < ppm->height; i++) {
        for (int j = 0; j < ppm->width; j++) {
            buffer[i * ppm->width + j] = ppm->data[(ppm->width - j - 1) * ppm->height + i];
        }
    }
    free(ppm->data);
    ppm->data = buffer;
}

void writePPM(char* fileName, PPM* ppm) {
    if (ppm == NULL) {
        return;
    }
    FILE* fout = fopen(fileName, "wb");
    if (fout == NULL) {
	    printf("Output file can't be created/rewritten\n");
	    return;
	}
    fprintf(fout, "%s\n%i %i\n%i\n", ppm->header, ppm->width, ppm->height, ppm->maxValue);
    fwrite(ppm->data, sizeof(PIXEL), ppm->width * ppm->height, fout);
    fclose(fout);
}