#ifndef FILE_SYSTEM_DEMO_FILESYSTEMTYPES_H
#define FILE_SYSTEM_DEMO_FILESYSTEMTYPES_H

struct file_metadata {
    char used;
    char name[27];
    short unsigned int base;
    short unsigned int size;
};


#endif //FILE_SYSTEM_DEMO_FILESYSTEMTYPES_H
