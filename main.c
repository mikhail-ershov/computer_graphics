#include <stdio.h>
#include <stdlib.h>
#include "manager.h"

int main(int argc, char* argv[]){
    if (argc < 4) {
        printf("Not enough arguments\n");
        exit(404);
    }
    if (argc > 4) {
        printf("Too many arguments\n");
        exit(404);
    }
    manageFile(argv[1], argv[2], atoi(argv[3]));
	return 0;
}
