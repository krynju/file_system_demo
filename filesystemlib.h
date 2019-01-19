
#ifndef FILE_SYSTEM_DEMO_FILESYSTEMLIB_H
#define FILE_SYSTEM_DEMO_FILESYSTEMLIB_H

#include "filesystemtypes.h"

int create_filesystem(char *);

int add_file(char *, char *);

int find_empty_metadata_slot(char *);

int find_empty_data_space(char *filesystem_name, unsigned int size);

#endif //FILE_SYSTEM_DEMO_FILESYSTEMLIB_H
