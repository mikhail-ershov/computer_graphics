#include <stdio.h>
#include <stdlib.h>
#include "pgm.h"

PGM* readPGM(char* fileName){
	FILE* fin = fopen(fileName, "rb");
	if (fin == NULL) {
	    printf("File can't be opened\n");
	    return NULL;
	}
	PGM* pgm = (PGM*) malloc(sizeof(PGM));
	if (pgm == NULL) {
	    printf("Malloc error happened\n");
	    return NULL;
	}
	if (fscanf(fin, "%s%d%d%d", pgm->header, &(pgm->width), &(pgm->height), &(pgm->maxValue)) < 4) {
        printf("Header reading error happened\n");
        free(pgm);
        return NULL;
	}
	if (fgetc(fin) == EOF) {
	    printf("Data reading error happened\n");
	}
	pgm->data = malloc(pgm->height * pgm->width);
	if (pgm->data == NULL) {
	    printf("Malloc error happened\n");
	    free(pgm);
	}
	if (fread(pgm->data, 1, pgm->width * pgm->height, fin) < pgm->width * pgm->height) {
	    printf("Data reading error happened\n");
	    free(pgm->data);
	    free(pgm);
	    return NULL;
	}
	if (fclose(fin) != 0) {
	    printf("Can't close file\n");
	    free(pgm->data);
	    free(pgm);
	    return NULL;
	}
	return pgm;
}

void inversePGM(PGM* pgm){
    if (pgm == NULL) {
        return;
    }
	for (int i = 0; i < pgm->height * pgm->height; i++) {
        pgm->data[i] = pgm->maxValue - pgm->data[i];
	}
}

void reflectPGM_V(PGM* pgm) {
    if (pgm == NULL) {
        return;
    }
	for (int i = 0; i < pgm->height / 2; i++) {
		for (int j = 0 ; j < pgm->width; j++) {
		    uchar tmp = pgm->data[i * pgm->width + j];
			pgm->data[i * pgm->width + j] = pgm->data[(pgm->height - i - 1) * pgm->width + j];
            pgm->data[(pgm->height - i - 1) * pgm->width + j] = tmp;
		}
	}
}

void reflectPGM_H(PGM* pgm) {
    if (pgm == NULL) {
        return;
    }
	for (int i = 0; i < pgm->height; i++) {
		for (int j = 0 ; j < pgm->width / 2; j++) {
		    uchar tmp = pgm->data[i * pgm->width + j];
			pgm->data[i * pgm->width + j] = pgm->data[i * pgm->width + pgm->width - j - 1];
            pgm->data[i * pgm->width + pgm->width - j - 1] = tmp;
		}
	}
}

void rotatePGM_L(PGM* pgm){
    if (pgm == NULL) {
        return;
    }
    int tmp = pgm->width;
    pgm->width = pgm->height;
    pgm->height = tmp;
    uchar* buffer = malloc(pgm->width * pgm->height);
    if (buffer == NULL) {
        printf("Malloc error happened\n");
        free(pgm->data);
        free(pgm);
        pgm = NULL;
        return;
    }
    for (int i = 0; i < pgm->height; i++) {
        for (int j = 0; j < pgm->width; j++) {
            buffer[i * pgm->width + j] = pgm->data[j * pgm->height + pgm->height - i - 1];
        }
    }
    free(pgm->data);
    pgm->data = buffer;
}

void rotatePGM_R(PGM* pgm){
    if (pgm == NULL) {
        return;
    }
    int tmp = pgm->width;
    pgm->width = pgm->height;
    pgm->height = tmp;
    uchar* buffer = malloc(pgm->width * pgm->height);
    if (buffer == NULL) {
        printf("Malloc error happened\n");
        free(pgm->data);
        free(pgm);
        pgm = NULL;
        return;
    }
    for (int i = 0; i < pgm->height; i++) {
        for (int j = 0 ; j < pgm->width; j++) {
            buffer[i * pgm->width + j] = pgm->data[(pgm->width - j - 1) * pgm->height + i];
        }
    }
    free(pgm->data);
    pgm->data = buffer;
}

void writePGM(char* fileName, PGM* pgm){
    if (pgm == NULL) {
        return;
    }
	FILE* fout = fopen(fileName, "wb");
	if (fout == NULL) {
	    printf("Output file can't be created/rewritten\n");
	}
	fprintf(fout, "%s\n%i\n%i\n%i\n", pgm->header, pgm->width, pgm->height, pgm->maxValue);
	fwrite(pgm->data, 1, pgm->width * pgm->height, fout);
	fclose(fout);
}