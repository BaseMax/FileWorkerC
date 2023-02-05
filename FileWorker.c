#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

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

char* skip_string(char* line, int length, int skip)
{
    int i = 0;
    char* result = malloc(sizeof(char) * (length - skip));

    for (int j = skip + 1; j < length; j++) result[i++] = line[j];

    result[i] = '\0';
    return result;
}

char* copystr(char* line, int length)
{
    char* result = malloc(sizeof(char) * length);
    for (int i = 0; i < length; i++) result[i] = line[i];
    result[length] = '\0';
    return result;
}

char **split_string(char *input, char sep, int *count)
{
    int len = strlen(input);

    char **result = malloc(sizeof(char*) * len);
    char *token = strtok(input, &sep);

    int i = 0;
    while (token != NULL) {
        result[i++] = token;
        token = strtok(NULL, &sep);
    }

    result[i] = NULL;
    *count = i;
    return result;
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

int dir_exists(char* dirname)
{
    DWORD ftyp = GetFileAttributesA(dirname);
    if (ftyp == INVALID_FILE_ATTRIBUTES) {
        return 0;
    }
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
        return 1;
    }
    return 0;
}

void mkdir(char* path)
{
    if (!dir_exists(path)) {
        CreateDirectory(path, NULL);
    }
}

void createfile(char* filename)
{
    int count;
    char *temp = copystr(filename, strlen(filename));
    char **steps = split_string(filename, '/', &count);

    // e.g: a/b/c/d.txt
    // checking dirs ['a/', 'a/b/', 'a/b/c/']
    // checking file 'a/b/c/d.txt'
    char* path = malloc(sizeof(char) * 512);
    strcpy(path, "");
    for (int i = 0; i < count - 1; i++) {
        strcat(path, steps[i]);

        if (path[strlen(path) - 1] != '/') strcat(path, "/");

        printf("checking dir `%s`\n", path);
        if (!dir_exists(path)) mkdir(path);
    }

    printf("checking file `%s`\n", temp);

    FILE* file = fopen(temp, "w");
    fclose(file);
}

char *file_read(char *filepath)
{
	FILE* file = fopen(filepath, "rb");
	if (file == NULL) {
		printf("Error: Could not open file \"%s\".", filepath);
		exit(74);
	}

	fseek(file, 0L, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);

	char* buffer = (char*)malloc(fileSize + 1);
	if (buffer == NULL) {
		printf("Error: Not enough memory to read \"%s\".", filepath);
		exit(74);
	}

	size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
	if (bytesRead < fileSize) {
		printf("Error: Could not read the \"%s\".", filepath);
		exit(74);
	}

	buffer[bytesRead] = '\0';

	fclose(file);
	return buffer;
}

char *putstring_at(char *contents, char *str, int line, int index)
{
    char *start = contents;
    int curr_line = 1;
    int curr_index = 0;
    int len = strlen(contents);
    char *new_contents = (char*)malloc(len + strlen(str) + 1);
    char *new_contents_ptr = new_contents;

    char *prev_char = NULL;
    char *curr_char = contents;
    int prev_newline = 0;

    while (*contents) {
        if (curr_line == line && curr_index == index) {
            strcpy(new_contents_ptr, str);
            new_contents_ptr += strlen(str);
            if (*curr_char != '\n' || (prev_char != NULL && *prev_char != '\n')) {
                *new_contents_ptr = *curr_char;
                new_contents_ptr++;
            }
            prev_newline = (*curr_char == '\n');
        } else {
            *new_contents_ptr = *contents;
            new_contents_ptr++;
            prev_newline = (*contents == '\n');
        }

        if (*contents == '\n') {
            curr_line++;
            curr_index = 0;
        } else {
            curr_index++;
        }
        prev_char = contents;
        curr_char = contents + 1;
        contents++;
    }

    if (curr_line == line && curr_index == index) {
        strcpy(new_contents_ptr, str);
        new_contents_ptr += strlen(str);
    }

    if (!prev_newline) {
        *new_contents_ptr = '\0';
    } else {
        *new_contents_ptr = '\n';
        new_contents_ptr++;
        *new_contents_ptr = '\0';
    }

    free(start);
    return new_contents;
}

void insertstr(char* filename, char* str, int line, int index)
{
    if (file_exists(filename)) {
        printf("insertstr: file `%s` exists\n", filename);
        char *contents = file_read(filename);
        printf("Content is '%s'\n", contents);
        contents = putstring_at(contents, str, line, index);
        printf("New Content is '%s'\n", contents);

        FILE* file = fopen(filename, "w");
        fprintf(file, "%s", contents);
        fclose(file);
    } else {
        printf("insertstr: file `%s` does not exist\n", filename);
    }
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
                for (int i = 0; i < args_count; i++) {
                    printf(" - %s\n", arguments[i]);
                }

                // Parsing commands
                if (strcmp(command, "createfile") == 0) {
                    // support --file <value>
                    if (args_count == 0) {
                        printf("createfile: invalid arguments try `--file <name>`\n");
                    } else {
                        char* name = NULL;
                        for (int i = 0; i < args_count; i++) {
                            if (strcmp(arguments[i], "--file") == 0) {
                                // printf("createfile: %s\n", arguments[i + 1]);
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
                } else if (strcmp(command, "insertstr") == 0) {
                    // support: --file name --str value --pos 2:3
                    if (args_count == 0) {
                        printf("insertstr: invalid arguments try `--file <name> --str <value> --pos <line>:<index>`\n");
                    } else {
                        char* name = NULL;
                        char* value = NULL;
                        int line = -1;
                        int index = -1;
                        for (int i = 0; i < args_count; i++) {
                            if (strcmp(arguments[i], "--file") == 0) {
                                // printf("createfile: %s\n", arguments[i + 1]);
                                if (i + 1 >= args_count) {
                                    break;
                                }
                                name = arguments[i + 1];
                            } else if (strcmp(arguments[i], "--str") == 0) {
                                // printf("createfile: %s\n", arguments[i + 1]);
                                if (i + 1 >= args_count) {
                                    break;
                                }
                                value = arguments[i + 1];
                            } else if (strcmp(arguments[i], "--pos") == 0) {
                                // printf("createfile: %s\n", arguments[i + 1]);
                                if (i + 1 >= args_count) {
                                    break;
                                }
                                char* pos = arguments[i + 1];
                                char** pos_args = split_string(pos, ':', &args_count);
                                if (args_count == 2) {
                                    line = atoi(pos_args[0]);
                                    index = atoi(pos_args[1]);
                                }
                            }
                        }
                        if (name == NULL || value == NULL || line == -1 || index == -1) {
                            printf("insertstr: invalid arguments try `--file <name> --str <value> --pos <line>:<index>`\n");
                        } else insertstr(name, value, line, index);
                    }
                } else if (strcmp(command, "cat") == 0) {
                    // handle --file name
                    if (args_count == 0) {
                        printf("cat: invalid arguments try `--file <name>`\n");
                    } else {
                        char* name = NULL;
                        for (int i = 0; i < args_count; i++) {
                            if (strcmp(arguments[i], "--file") == 0) {
                                // printf("createfile: %s\n", arguments[i + 1]);
                                if (i + 1 >= args_count) {
                                    break;
                                }
                                name = arguments[i + 1];
                                break;
                            }
                        }
                        if (name == NULL) {
                            printf("cat: invalid arguments try `--file <name>`\n");
                        } else {
                            if (file_exists(name)) {
                                char* contents = file_read(name);
                                printf("%s", contents);
                            } else {
                                printf("cat: file `%s` does not exist\n", name);
                            }
                        }
                    }
                }
                printf("Checking %s.%s\n", command, args);
                // parse(2, (char*[]){command, args});
            }
        }
    }

    return 0;
}
