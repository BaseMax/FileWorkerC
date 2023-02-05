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

// backward mean remove while go back
// forward mean remove while go forward
char *remove_at(char *contents, int line, int index, int size, int mode)
{
    char *start = contents;
    int curr_line = 1;
    int curr_index = 0;
    int len = strlen(contents);
    char *new_contents = (char*)malloc(len + 1);
    char *new_contents_ptr = new_contents;

    char *prev_char = NULL;
    char *curr_char = contents;
    int prev_newline = 0;

    while (*contents) {
        if (curr_line == line && curr_index == index) {
            if (mode == 0) {
                contents += size;
            } else if (mode == 1) {
                contents += size;
                curr_index += size;
            } else if (mode == 2) {
                contents += size;
                curr_index += size;
                curr_line += size;
            }
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

void removestr(char *name, int line, int index, int size, int mode)
{
    if (file_exists(name)) {
        printf("removestr: file `%s` exists\n", name);
        char *contents = file_read(name);
        printf("Content is '%s'\n", contents);
        contents = remove_at(contents, line, index, size, mode);
        printf("New Content is '%s'\n", contents);

        FILE* file = fopen(name, "w");
        fprintf(file, "%s", contents);
        fclose(file);
    } else {
        printf("removestr: file `%s` does not exist\n", name);
    }
}

// return index of the first char of the string
// otherwise return -1
int search_string(char *contents, char *looking_for)
{
    int len = strlen(contents);
    int looking_for_len = strlen(looking_for);
    int i = 0;
    int j = 0;
    while (i < len) {
        if (contents[i] == looking_for[j]) {
            j++;
            if (j == looking_for_len) {
                return i - j + 1;
            }
        } else {
            j = 0;
        }
        i++;
    }
    return -1;
}

int *search_string_all(char *contents, char *value)
{
    int len = strlen(contents);
    int value_len = strlen(value);
    int i = 0;
    int j = 0;
    int count = 0;
    int *result = (int*)malloc(sizeof(int) * 100);
    while (i < len) {
        if (contents[i] == value[j]) {
            j++;
            if (j == value_len) {
                result[count] = i - j + 1;
                count++;
                j = 0;
            }
        } else {
            j = 0;
        }
        i++;
    }
    result[count] = -1;
    return result;
}

int search_string_count(char *contents, char *value)
{
    int len = strlen(contents);
    int value_len = strlen(value);
    int i = 0;
    int j = 0;
    int count = 0;
    while (i < len) {
        if (contents[i] == value[j]) {
            j++;
            if (j == value_len) {
                count++;
                j = 0;
            }
        } else {
            j = 0;
        }
        i++;
    }
    return count;
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
                                i++;
                            } else if (strcmp(arguments[i], "--str") == 0) {
                                // printf("createfile: %s\n", arguments[i + 1]);
                                if (i + 1 >= args_count) {
                                    break;
                                }
                                value = arguments[i + 1];
                                i++;
                            } else if (strcmp(arguments[i], "-pos") == 0) {
                                // printf("createfile: %s\n", arguments[i + 1]);
                                if (i + 1 >= args_count) {
                                    break;
                                }
                                char* pos = arguments[i + 1];
                                int pos_args_count;
                                char** pos_args = split_string(pos, ':', &pos_args_count);
                                if (args_count == 2) {
                                    line = atoi(pos_args[0]);
                                    index = atoi(pos_args[1]);
                                }
                                i++;
                            }
                        }
                        if (name == NULL || value == NULL || line == -1 || index == -1) {
                            printf("insertstr: invalid arguments try `--file <name> --str <value> -pos <line>:<index>`\n");
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
                } else if (strcmp(command, "find") == 0) {
                    // handling -–str <str> -–file <file name>
                    // -count is a optional argument
                    // -at is a optional argument
                    int mode = 0;
                    int at = -1;
                    if (args_count == 0) {
                        printf("find: invalid arguments try `--str <value> --file <name>`\n");
                    } else {
                        char* name = NULL;
                        char* value = NULL;
                        int error = 0;
                        for (int i = 0; i < args_count; i++) {
                            if (strcmp(arguments[i], "--file") == 0) {
                                // printf("createfile: %s\n", arguments[i + 1]);
                                if (i + 1 >= args_count) {
                                    break;
                                }
                                name = arguments[i + 1];
                                i++;
                            } else if (strcmp(arguments[i], "--str") == 0) {
                                // printf("createfile: %s\n", arguments[i + 1]);
                                if (i + 1 >= args_count) {
                                    break;
                                }
                                value = arguments[i + 1];
                                i++;
                            } else if (strcmp(arguments[i], "-count") == 0) {
                                mode = 1;
                            } else if (strcmp(arguments[i], "-at") == 0) {
                                if (i + 1 >= args_count) {
                                    error = 1;
                                    break;
                                }
                                mode = 2;
                                at = atoi(arguments[i + 1]);
                                i++;
                            }
                        }
                        if (error == 1 || name == NULL || value == NULL) {
                            printf("find: invalid arguments try `--str <value> --file <name>` with `-count` or `-at n`\n");
                        } else {
                            if (file_exists(name)) {
                                char* contents = file_read(name);

                                if (mode == 0) {
                                    int offset = search_string(contents, value);
                                    if (offset == -1) {
                                        printf("find: string `%s` not found in file `%s`\n", value, name);
                                    } else {
                                        printf("find: string `%s` found in file `%s` at offset %d\n", value, name, offset);
                                    }
                                } else if (mode == 1) {
                                    printf("mode is 1\n");
                                    int count = search_string_count(contents, value);
                                    printf("c is %d\n", count);
                                    printf("find: string `%s` found in file `%s` %d times\n", value, name, count);
                                } else if (mode == 2) {
                                    int* offsets = search_string_all(contents, value);
                                    int count = search_string_count(contents, value);
                                    if (count == 0 || offsets == NULL || offsets[0] == -1) {
                                        printf("find: string `%s` not found in file `%s`\n", value, name);
                                        continue;
                                    } else if (at >= count) {
                                        printf("find: string `%s` found in file `%s` but there is no %dth occurence\n", value, name, at);
                                        continue;
                                    } else {
                                        printf("find: string `%s` found in file `%s` at offsets: ", value, name);
                                        for (int i = 0; i < count; i++) {
                                            printf("%d ", offsets[i]);
                                        }
                                        printf("\n");
                                    }
                                }
                            }
                        }
                    }
                } else if (strcmp(command, "removestr") == 0) {
                    // handling --file val -pos line:pos -size n -f
                    // handling --file val -pos line:pos -size n -b
                    // so -f and -b are optional but it's needed to specify one of them
                    if (args_count == 0) {
                        printf("removestr: invalid arguments try `--file <name> -pos <line>:<index> -size <n> -f/-b`\n");
                    } else {
                        char* name = NULL;
                        int line = -1;
                        int index = -1;
                        int size = -1;
                        int mode = -1;
                        printf("args count: %d\n", args_count);
                        printf("last arg is %s\n", arguments[args_count - 1]);
                        int i;
                        for (i = 0; i < args_count; i++) {
                            // printf("==> removestr: %s\n", arguments[i]);
                            if (strcmp(arguments[i], "--file") == 0) {
                                // printf("file createfile: %s\n", arguments[i + 1]);
                                if (i + 1 >= args_count) {
                                    break;
                                }
                                name = arguments[i + 1];
                                i++;
                            } else if (strcmp(arguments[i], "-pos") == 0) {
                                // printf("pos createfile: %s\n", arguments[i + 1]);
                                if (i + 1 >= args_count) {
                                    break;
                                }
                                char* pos = arguments[i + 1];
                                int pos_args_count;
                                char** pos_args = split_string(pos, ':', &pos_args_count);
                                if (pos_args_count == 2) {
                                    line = atoi(pos_args[0]);
                                    index = atoi(pos_args[1]);
                                }
                                i++;
                            } else if (strcmp(arguments[i], "-size") == 0) {
                                // printf("size createfile: %s\n", arguments[i + 1]);
                                if (i + 1 >= args_count) {
                                    break;
                                }
                                size = atoi(arguments[i + 1]);
                                i++;
                            } else if (strcmp(arguments[i], "-f") == 0) {
                                mode = 0;
                            } else if (strcmp(arguments[i], "-b") == 0) {
                                mode = 1;
                            }
                        }

                        if (name == NULL || line == -1 || index == -1 || size == -1 || mode == -1) {
                            printf("removestr: invalid arguments try `--file <name> -pos <line>:<index> -size <n> -f/-b`\n");
                        } else {
                            if (file_exists(name)) {
                                removestr(name, line, index, size, mode);
                            } else {
                                printf("removestr: file `%s` does not exist\n", name);
                            }
                        }
                    }
                }
                printf("Checking %s.%s\n", command, args);
            }
        }
    }

    return 0;
}
