
#include <memory.h>
#include "filesystemlib.h"


int main(int argc, char *argv[]) {
    if (argc < 3) {
        return 1;
    }

    if (argc == 3) {
        if (strcmp(argv[1], "-c\0") == 0) {
            create_filesystem(argv[2]);
        }
    }

    if (argc == 5) {
        if (strcmp(argv[1], "-f\0") == 0) {
            add_file(argv[2], argv[4]);
        }
    }

    return 0;
}