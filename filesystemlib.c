#include <stdio.h>
#include "filesystemlib.h"

void test() {

}

int create_filesystem(char *name) {
    FILE *file;
    printf("Creating filesystem: %.20s ... ", name);
    file = fopen(name, "w");
//    fseek(file, 10, SEEK_CUR);
//    fputc(0xEE,file);

    fclose(file);
    printf("success\n");
    return 0;
}

int write_file_metadata(char *filesystem_name, char *filename) {
    struct file_metadata f_meta;
    printf("Writing file metadata: ... ");

    printf("success\t");
    return 0;
}

