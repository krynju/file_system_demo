
#ifndef FILE_SYSTEM_DEMO_FILESYSTEMLIB_H
#define FILE_SYSTEM_DEMO_FILESYSTEMLIB_H

#include "filesystemtypes.h"

void test();

int create_filesystem(char *);

int add_file(char *, char *);

int find_empty_metadata_slot(char *filesystem_name);

#endif //FILE_SYSTEM_DEMO_FILESYSTEMLIB_H
