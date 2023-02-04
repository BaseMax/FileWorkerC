#include <stdio.h>
#include <stdlib.h>

void invalid()
{
    printf("invalid command\n");
}

void help()
{
    printf("Supported commands:\n");
    printf(" - help\n");
    printf("\n");
}

int file_exists(char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

void createfile(char* filename)
{
    FILE* file = fopen(filename, "w");
    fclose(file);
}

void insertstr(char* filename, char* str, int line, int index)
{
    if (file_exists(filename)) {
        printf("insertstr: file `%s` exists\n", filename);
    } else {
        printf("insertstr: file `%s` does not exist\n", filename);
    }
}

void parse(int argc, char** argv)
{
    char* command = argv[0];

    // printf("%d\n", argc);
    // for (int i = 0; i < argc; i++) {
    //     printf("%s\n", argv[i]);
    // }

    if (strcmp(command, "help") == 0) {
        help();
    } else if (strcmp(command, "createfile") == 0) {
        if (argc == 2 && strcmp(argv[0], "--file") == 0) {
            createfile(argv[1]);
        } else {
            printf("createfile: invalid arguments try `--file <name>`\n");
        }
    } else if (strcmp(command, "insertstr") == 0) {
        // support: --file name --str value --pos 2:3
        if (argc == 6 && strcmp(argv[0], "--file") == 0 && strcmp(argv[2], "--str") == 0 && strcmp(argv[4], "--pos") == 0) {
            insertstr(argv[1], argv[3], 2, 3);
        } else {
            printf("insertstr: invalid arguments try `--file <name> --str <value> --pos <line>:<index>`\n");
        }
    } else if (strcmp(command, "cat") == 0) {
        cat(argc, argv);
    // find –str ”an expression with \* but not wildcard” –file a.txt
    // find –str fire –file a.txt -count
    // find –str fire –file a.txt -at 3
    // find –str ”salam khubi*” –file a.txt -byword
    // find –str ”difficult project” –file a.txt -all -byword
    } else if (strcmp(command, "find") == 0) {
        cat(argc, argv);
    // replace –str1 <str> –str2 <str> –file <file name> [-at <num> | -all]
    } else if (strcmp(command, "replace") == 0) {
        replace(argc, argv);
    } else if (strcmp(command, "compare") == 0) {
        compare(argc, argv);
    } else {
        invalid();
    }
}

int main(int argc, char** argv)
{
    if (argc == 1) {
        help();
    } else if (argc >= 2) {
        argv++;
        parse(argc - 1, argv);
    }

    return 0;
}
