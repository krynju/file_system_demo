#ifndef FILE_SYSTEM_DEMO_FILESYSTEMTYPES_H
#define FILE_SYSTEM_DEMO_FILESYSTEMTYPES_H

#define METADATA_SIZE 4096
#define DATA_SIZE 65536
#define METADATA_MAX_ENTRIES 128

struct FileMetadata {
    char used;
    char name[27];
    short unsigned int base;
    short unsigned int size;
};


#endif //FILE_SYSTEM_DEMO_FILESYSTEMTYPES_H
