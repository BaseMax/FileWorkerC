#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        // cat(argc, argv);
    // find –str ”an expression with \* but not wildcard” –file a.txt
    // find –str fire –file a.txt -count
    // find –str fire –file a.txt -at 3
    // find –str ”salam khubi*” –file a.txt -byword
    // find –str ”difficult project” –file a.txt -all -byword
    } else if (strcmp(command, "find") == 0) {
        // cat(argc, argv);
    // replace –str1 <str> –str2 <str> –file <file name> [-at <num> | -all]
    } else if (strcmp(command, "replace") == 0) {
        // replace(argc, argv);
    } else if (strcmp(command, "compare") == 0) {
        // compare(argc, argv);
    } else {
        invalid();
    }
}

char* skip_string(char* line, int length, int skip)
{
    int i = 0;
    char* result = malloc(sizeof(char) * (length - skip));

    for (int j = skip + 1; j < length; j++) result[i++] = line[j];

    result[i] = '\0';
    return result;
}

char **split_string(char *input, char sep, int *count)
{
    int i = 0;
    char **result = malloc(sizeof(char*) * 512);
    char *token = strtok(input, &sep);

    while (token != NULL) {
        result[i++] = token;
        token = strtok(NULL, &sep);
    }

    result[i] = NULL;
    *count = i;
    return result;
}

int main(int argc, char** argv)
{
    int flag = 1;
    while (flag == 1) {
        printf("FileWorker> ");
        char* input = malloc(512);
        fgets(input, 512, stdin);
        int len = strlen(input);
        if (input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        char* line = malloc(sizeof(char) * len);
        strcpy(line, input);
        line[len] = '\0';
        char* command = strtok(input, " ");

        if (command == NULL) {
            printf("Empty\n");
        } else {
            char* args = skip_string(line, len, strlen(command));
            int args_count = 0;
            char** arguments = split_string(args, ' ', &args_count);

            if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0 || strcmp(command, "q") == 0) {
                flag = 0;
            } else if (strcmp(command, "help") == 0 || strcmp(command, "h") == 0) {
                help();
            } else {
                // Parsing commands
                if (strcmp(command, "createfile") == 0) {
                    // support --file <value>
                    if (args_count == 0) {
                        printf("createfile: invalid arguments try `--file <name>`\n");
                    } else {
                        char* name = NULL;
                        for (int i = 0; i < args_count; i++) {
                            if (strcmp(arguments[i], "--file") == 0) {
                                if (i + 1 >= args_count) {
                                    break;
                                }
                                name = arguments[i + 1];
                                break;
                            }
                        }

                        if (name == NULL) {
                            printf("createfile: invalid arguments try `--file <name>`\n");
                        } else createfile(name);
                    }
                }
                printf("Checking %s.%s\n", command, args);
                // parse(2, (char*[]){command, args});
            }
        }
    }

    return 0;
}
