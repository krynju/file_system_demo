#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "filesystemlib.h"

void test() {

}

int create_filesystem(char *name) {
    FILE *file = fopen(name, "w");
    printf("Creating filesystem: %.20s ... ", name);
    char *p = malloc(4096);
    memset(p, 0, 4096);
    fwrite(p, 4096, 1, file);
    fclose(file);
    free(p);
    printf("success\n");
    return 0;
}

int add_file(char *filesystem_name, char *filename) {
    struct file_metadata f_meta;
    int meta_offset;
    FILE *filesystem_handle;

    printf("Adding file %.30s to %.30s ...\n", filename, filesystem_name);
    printf("> Writing file metadata: ... ");
    //check empty space first
    meta_offset = find_empty_metadata_slot(filesystem_name);
    if (meta_offset < 0) {
        printf("failed\n");
        printf(">Failed to find an empty metadata spot, remove a file to create a free slot\n");
        return -1;
    }

    f_meta.used = 1;
    sscanf(filename, "%26s", f_meta.name);

    filesystem_handle = fopen(filesystem_name, "r+");
    fseek(filesystem_handle, meta_offset, SEEK_SET);
    fwrite(&f_meta, 1, 32, filesystem_handle);
    fclose(filesystem_handle);

    printf("success\n");
    printf(">> Added file metadata at slot: %3d, offset: %4d\n", meta_offset / 32, meta_offset);
    printf("Done\n");
    return 0;
}

int find_empty_metadata_slot(char *filesystem_name) {
    FILE *filesystem_handle = fopen(filesystem_name, "r");
    int offset = 0;

    while (offset < 4096) {
        fseek(filesystem_handle, offset, SEEK_SET);
        if (getc(filesystem_handle) == 0x00) {
            fclose(filesystem_handle);
            return offset;
        }
        offset += 32;
    }

    fclose(filesystem_handle);
    return -1;
}


