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
    printf("done\n");
    return 0;
}

int add_file(char *filesystem_name, char *file_name) {
    struct FileMetadata file_metadata;
    int metadata_offset;
    FILE *filesystem_handle, *file_handle;

    printf("Adding file %.30s to %.30s ...\n", file_name, filesystem_name);

    printf("> Checking if file is already in the filesystem ... ");
    metadata_offset = find_file(filesystem_name, file_name);
    if (metadata_offset != -1) {
        printf("failed\n");
        fprintf(stderr, "File exists in the filesystem, remove it before adding it\n");
        return -1;
    }
    printf("not found\n # File not found in the filesystem, continue operation\n");

    printf("> Checking file size ... ");
    file_handle = fopen(file_name, "r");
    fseek(file_handle, 0, SEEK_END);
    long filesize = ftell(file_handle);
    fclose(file_handle);
    printf("done\n # filesize: %.10li\n", filesize);


    printf("> Looking for free space ... ");
    int base = find_empty_data_space(filesystem_name, (unsigned int) filesize);
    if (base == -1) {
        printf("failed\n");
        fprintf(stderr, "Failed to find empty data space large enough to fit the file\n");
        return -1;
    }
    printf("done\n # found at offset: %.10d\n", base);


    printf("> Looking for free metadata slot ... ");
    metadata_offset = find_empty_metadata_slot(filesystem_name);
    if (metadata_offset < 0) {
        printf("failed\n");
        fprintf(stderr, "Failed to find an empty metadata spot, remove a file to create a free slot\n");
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
                taken_space[j + 1][1] = temp[1];
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

int remove_file(char *filesystem_name, char *file_name) {
    FILE *filesystem_handle;
    int metadata_offset;

    printf("Removing file %.27s\n", file_name);

    printf("> Finding file metadata ... ");
    metadata_offset = find_file(filesystem_name, file_name);
    if (metadata_offset == -1) {
        printf("failed\n");
        fprintf(stderr, "File not found ");
        return -1;
    }
    printf("done\n # File found at offset %.10d\n", metadata_offset);

    printf("> Deactivating file entry in the filesystem metadata table ... ");
    filesystem_handle = fopen(filesystem_name, "r+");
    fseek(filesystem_handle, metadata_offset, SEEK_SET);
    fputc(0x00, filesystem_handle);
    fclose(filesystem_handle);
    printf("done\n # Metadata deactivated successfully at %.10d\n", metadata_offset);

    printf("Done\n");
    return 0;
}

int find_file(char *filesystem_name, char *file_name) {
    char metadata[METADATA_SIZE];

    FILE *filesystem_handle = fopen(filesystem_name, "r");
    fread(metadata, METADATA_SIZE, 1, filesystem_handle);
    fclose(filesystem_handle);

    for (int i = 0; i < METADATA_SIZE; i += 32)
        if (metadata[i] == 0x01 && 0 == strcmp(file_name, &metadata[i + 1]))
            return i;

    return -1;
}

int remove_filesystem(char *filesystem_name) {
    if (remove(filesystem_name) != 0)
        fprintf(stderr, "File not found \n");
    printf("Successfully removed the filesystem");
    return 0;
}

int copy_file_from(char *filesystem_name, char *file_name, char *output_name) {
    int metadata_offset;
    unsigned short int base, size;
    char metadata[32];
    FILE *filesystem_handle, *output_handle;

    printf("Copying file %.10s from the filesystem %.10s to %.10s\n", file_name, filesystem_name, output_name);

    printf("> Finding file metadata ... ");
    metadata_offset = find_file(filesystem_name, file_name);
    if (metadata_offset == -1) {
        printf("failed\n");
        fprintf(stderr, "File not found ");
        return -1;
    }
    printf("done\n # File found at offset %.10d\n", metadata_offset);

    printf("> Copying the file ...");
    filesystem_handle = fopen(filesystem_name, "r");
    fseek(filesystem_handle, metadata_offset, SEEK_SET);
    fread(metadata, 32, 1, filesystem_handle);
    base = *((unsigned short *) &metadata[28]);
    size = *((unsigned short *) &metadata[30]);

    char *p = malloc((size_t) size);
    fseek(filesystem_handle, METADATA_SIZE + base, SEEK_SET);
    fread(p, (size_t) size, 1, filesystem_handle);
    fclose(filesystem_handle);

    output_handle = fopen(output_name, "w");
    fwrite(p, (size_t) size, 1, output_handle);
    fclose(output_handle);
    free(p);
    printf("done\n # File copied successfully\n");

    printf("Done\n");
    return 0;
}

int display_catalogue(char *filesystem_name) {
    char metadata[METADATA_SIZE];
    int file_count = 0, total_size = 0;
    FILE *filesystem_handle = fopen(filesystem_name, "r");
    fread(metadata, METADATA_SIZE, 1, filesystem_handle);
    fclose(filesystem_handle);

    printf("name\t| base \t| size \n");
    for (int i = 0; i < METADATA_SIZE; i += 32)
        if (metadata[i] == 0x01) {
            printf("%.27s\t", &metadata[i + 1]);
            printf("%5d\t", *((unsigned short *) &metadata[i + 28]));
            printf("%5d", *((unsigned short *) &metadata[i + 30]));
            total_size += *((unsigned short *) &metadata[i + 30]);
            file_count++;
            printf("\n");
        }

    printf("files: %3d, total size: %5d / %5d, usage: %2.3f %%\n", file_count, total_size, DATA_SIZE,
           100.0 * total_size / DATA_SIZE);
    return 0;
}

int display_map(char *filesystem_name) {
    char metadata[METADATA_SIZE];
    unsigned short int space[METADATA_MAX_ENTRIES][2];

    FILE *filesystem_handle = fopen(filesystem_name, "r");
    fread(metadata, METADATA_SIZE, 1, filesystem_handle);
    fclose(filesystem_handle);
    int file_count = 0;

    for (int i = 0; i < METADATA_SIZE; i += 32)
        if (metadata[i] == 0x01) {
            space[file_count][0] = *((unsigned short *) &metadata[i + 28]);
            space[file_count][1] = *((unsigned short *) &metadata[i + 30]);
            file_count++;
        }

    for (int i = 0; i < file_count - 1; i++)
        for (int j = 0; j < file_count - i - 1; j++)
            if (space[j][0] > space[j + 1][0]) {
                unsigned short int temp[2];
                temp[0] = space[j][0];
                temp[1] = space[j][1];
                space[j][0] = space[j + 1][0];
                space[j][1] = space[j + 1][1];
                space[j + 1][0] = temp[0];
                space[j + 1][1] = temp[1];
            }

    printf("| state\t| base\t| end\t| size\n");

    if (file_count == 0) {
        printf("| empty\t| 0\t| %d\t| %d\n",
               DATA_SIZE,
               DATA_SIZE
        );
        return 0;
    }

    if (space[0][0] != 0) {
        printf("| empty\t| 0\t\t| %d\t| %d\n",
               space[0][0],
               space[0][0]
        );
    }

    int c_base = 0, c_size = 0, c_end = 0;
    c_base = space[0][0];
    c_end = space[0][0] + space[0][1];
    c_size = space[0][1];

    for (int i = 1; i < file_count; ++i) {
        if (space[i][0] - space[i - 1][0] - space[i - 1][1] != 0) {
            printf("| full\t| %d\t| %d\t| %d\n",
                   c_base,
                   c_end,
                   c_size
            );

            printf("| empty\t| %d\t| %d\t| %d\n",
                   space[i - 1][0] + space[i - 1][1],
                   space[i][0],
                   space[i][0] - space[i - 1][0] - space[i - 1][1]
            );

            c_base = space[i][0];
            c_end = space[i][0];
            c_size = 0;
        }
        c_end += space[i][1];
        c_size += space[i][1];

        if (i == file_count - 1)
            printf("| full\t| %d\t| %d\t| %d\n",
                   c_base,
                   c_end,
                   c_size
            );
    }

    if (DATA_SIZE - space[file_count - 1][0] - space[file_count - 1][1] != 0)
        printf("| empty\t| %d\t| %d\t| %d\n",
               space[file_count - 1][0] + space[file_count - 1][1],
               DATA_SIZE,
               DATA_SIZE - space[file_count - 1][0] - space[file_count - 1][1]
        );


    return 0;
}

int add_file_as(char *filesystem_name, char *file_name, char *output_name) {
    struct FileMetadata file_metadata;
    int metadata_offset;
    FILE *filesystem_handle, *file_handle;

    printf("Adding file %.30s to %.30s ...\n", file_name, filesystem_name);

    printf("> Checking if file is already in the filesystem ... ");
    metadata_offset = find_file(filesystem_name, output_name);
    if (metadata_offset != -1) {
        printf("failed\n");
        fprintf(stderr, "File exists in the filesystem, remove it before adding it\n");
        return -1;
    }
    printf("not found\n # File not found in the filesystem, continue operation\n");

    printf("> Checking file size ... ");
    file_handle = fopen(file_name, "r");
    fseek(file_handle, 0, SEEK_END);
    long filesize = ftell(file_handle);
    fclose(file_handle);
    printf("done\n # filesize: %.10li\n", filesize);


    printf("> Looking for free space ... ");
    int base = find_empty_data_space(filesystem_name, (unsigned int) filesize);
    if (base == -1) {
        printf("failed\n");
        fprintf(stderr, "Failed to find empty data space large enough to fit the file\n");
        return -1;
    }
    printf("done\n # found at offset: %.10d\n", base);


    printf("> Looking for free metadata slot ... ");
    metadata_offset = find_empty_metadata_slot(filesystem_name);
    if (metadata_offset < 0) {
        printf("failed\n");
        fprintf(stderr, "Failed to find an empty metadata spot, remove a file to create a free slot\n");
        return -1;
    }
    printf("done\n # found at slot: %3d, offset: %4d\n", metadata_offset / 32, metadata_offset);


    printf("> Writing file metadata: ... ");
    file_metadata.used = 1;
    sscanf(output_name, "%26s", file_metadata.name);
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

