
#ifndef FILE_SYSTEM_DEMO_FILESYSTEMLIB_H
#define FILE_SYSTEM_DEMO_FILESYSTEMLIB_H

#include "filesystemtypes.h"

int create_filesystem(char *filesystem_name);

int remove_filesystem(char *filesystem_name);

int add_file(char *filesystem_name, char *file_name);

int remove_file(char *filesystem_name, char *file_name);

int find_file(char *filesystem_name, char *file_name);

int find_empty_metadata_slot(char *filesystem_name);

int find_empty_data_space(char *filesystem_name, unsigned int size);

int copy_file_from(char *filesystem_name, char *file_name, char *output_name);

int display_catalogue(char *filesystem_name);

int display_map(char *filesystem_name);
#endif //FILE_SYSTEM_DEMO_FILESYSTEMLIB_H
