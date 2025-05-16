#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "management.h"



int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: ./fs [command] [args...]\n");
        return 1;
    }

    const char *cmd = argv[1];
    if (strcmp(cmd, "create") == 0 && argc == 4)
        create_disk(argv[2], atoi(argv[3]));
    else if (strcmp(cmd, "delete") == 0 && argc == 3)
        delete_disk(argv[2]);
    else if (strcmp(cmd, "copyin") == 0 && argc == 4)
        copy_in(argv[2], argv[3]);
    else if (strcmp(cmd, "copyout") == 0 && argc == 5)
        copy_out(argv[2], argv[3], argv[4]);
    else if (strcmp(cmd, "rm") == 0 && argc == 4)
        remove_file(argv[2], argv[3]);
    else if (strcmp(cmd, "ls") == 0 && argc == 3)
        list_directory(argv[2]);
    else if (strcmp(cmd, "about") == 0 && argc == 3)
        about_drive(argv[2]);
    else if (strcmp(cmd, "map") == 0 && argc == 3)
        show_map(argv[2]);
    else
        printf("Invalid command or arguments.\n");

    return 0;
}
