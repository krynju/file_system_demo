#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "filesystemlib.h"

int create_filesystem(char *name) {
    FILE *file = fopen(name, "w");
    printf("Creating filesystem: %.20s ... ", name);
    char *p = malloc(METADATA_SIZE + DATA_SIZE);
    memset(p, 0, METADATA_SIZE + DATA_SIZE);
    fwrite(p, METADATA_SIZE + DATA_SIZE, 1, file);
    fclose(file);
    free(p);
    printf("success\n");
    return 0;
}

int add_file(char *filesystem_name, char *file_name) {
    struct FileMetadata file_metadata;
    int metadata_offset;
    FILE *filesystem_handle, *file_handle;

    printf("Adding file %.30s to %.30s ...\n", file_name, filesystem_name);


    printf("> Checking file size ... ");
    file_handle = fopen(file_name, "r");
    fseek(file_handle, 0, SEEK_END);
    long filesize = ftell(file_handle);
    fclose(file_handle);
    printf("done\n # filesize: %.10li\n", filesize);


    printf("> Looking for free space ... ");
    int base = find_empty_data_space(filesystem_name, (unsigned int) filesize);
    if (base == -1) {
        printf("failed\n > Failed to find empty data space large enough to fit the file\n");
        return -1;
    }
    printf("done\n # found at offset: %.10d\n", base);


    printf("> Looking for free metadata slot ... ");
    metadata_offset = find_empty_metadata_slot(filesystem_name);
    if (metadata_offset < 0) {
        printf("failed\n > Failed to find an empty metadata spot, remove a file to create a free slot\n");
        return -1;
    }
    printf("done\n # found at slot: %3d, offset: %4d\n", metadata_offset / 32, metadata_offset);


    printf("> Writing file metadata: ... ");
    file_metadata.used = 1;
    sscanf(file_name, "%26s", file_metadata.name);
    file_metadata.base = (unsigned short) base;
    file_metadata.size = (unsigned short) filesize;
    filesystem_handle = fopen(filesystem_name, "r+");
    fseek(filesystem_handle, metadata_offset, SEEK_SET);
    fwrite(&file_metadata, 1, 32, filesystem_handle);
    fclose(filesystem_handle);
    printf("done\n # Added file metadata at slot: %3d, offset: %4d\n", metadata_offset / 32, metadata_offset);


    printf("> Writing file data: ... ");
    file_handle = fopen(file_name, "r");
    char *p = malloc((size_t) filesize);
    fread(p, (size_t) filesize, 1, file_handle);
    fclose(file_handle);
    filesystem_handle = fopen(filesystem_name, "r+");
    fseek(filesystem_handle, METADATA_SIZE + base, SEEK_SET);
    fwrite(p, (size_t) filesize, 1, filesystem_handle);
    fclose(filesystem_handle);
    free(p);
    printf("done\n # Added file data at offset: %.10i, bytecount: %li\n", base, filesize);


    printf("Done\n");
    return 0;
}

int find_empty_metadata_slot(char *filesystem_name) {
    FILE *filesystem_handle = fopen(filesystem_name, "r");
    int offset = 0;

    while (offset < METADATA_SIZE) {
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

int find_empty_data_space(char *filesystem_name, unsigned int size) {
    char metadata[METADATA_SIZE];
    unsigned short int taken_space[METADATA_MAX_ENTRIES][2];
    int file_count = 0;
    FILE *filesystem_handle = fopen(filesystem_name, "r");

    fread(metadata, METADATA_SIZE, 1, filesystem_handle);
    fclose(filesystem_handle);

    for (int i = 0; i < METADATA_SIZE; i += 32)
        if (metadata[i] == 0x01) {
            taken_space[file_count][0] = *((unsigned short *) &metadata[i + 28]);
            taken_space[file_count][1] = *((unsigned short *) &metadata[i + 30]);
            file_count++;
        }

    for (int i = 0; i < file_count - 1; i++)
        for (int j = 0; j < file_count - i - 1; j++)
            if (taken_space[j][0] > taken_space[j + 1][0]) {
                unsigned short int temp[2];
                temp[0] = taken_space[j][0];
                temp[1] = taken_space[j][1];
                taken_space[j][0] = taken_space[j + 1][0];
                taken_space[j][1] = taken_space[j + 1][1];
                taken_space[j + 1][0] = temp[0];
                taken_space[j + 1][1] = temp[0];
            }

    if (file_count == 0)
        return 0;

    if (taken_space[0][0] >= size)
        return 0;

    for (int i = 1; i < file_count - 1; ++i)
        if (taken_space[i][0] - taken_space[i - 1][0] - taken_space[i - 1][1] >= size)
            return taken_space[i - 1][0] + taken_space[i - 1][1];

    if (DATA_SIZE - taken_space[file_count - 1][0] - taken_space[file_count - 1][1] >= size)
        return taken_space[file_count - 1][0] + taken_space[file_count - 1][1];

    return -1;
}


