
#include <memory.h>
#include "filesystemlib.h"


int main(int argc, char *argv[]) {
    if (argc < 3) {
        return -1;
    }

    if (argc == 3) {
        if (strcmp(argv[1], "-c\0") == 0)
            return create_filesystem(argv[2]);
        if (strcmp(argv[1], "-d\0") == 0)
            return remove_filesystem(argv[2]);
        if (strcmp(argv[1], "-l\0") == 0)
            return display_catalogue(argv[2]);
        if (strcmp(argv[1], "-lr\0") == 0)
            return display_map(argv[2]);
    }


    if (argc == 5)
        if (strcmp(argv[1], "-f\0") == 0) {
            if (strcmp(argv[3], "-a\0") == 0)
                return add_file(argv[2], argv[4]);
            if (strcmp(argv[3], "-d\0") == 0)
                return remove_file(argv[2], argv[4]);
        }

    if (argc == 7)
        if (strcmp(argv[1], "-f\0") == 0) {
            if (strcmp(argv[3], "-c\0") == 0)
                if (strcmp(argv[5], "-o\0") == 0)
                    return copy_file_from(argv[2], argv[4], argv[6]);
            if (strcmp(argv[3], "-a\0") == 0)
                if (strcmp(argv[5], "-o\0") == 0)
                    return add_file_as(argv[2], argv[4], argv[6]);
        }



    return 0;
}