#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>


#define MAX_ARGS 64
#define MAX_PATH 1024

char *builtin_commands[] = {"cd", "pwd", "which", "exit"};
int num_builtins = sizeof(builtin_commands) / sizeof(char *);

//Addition
void execute_command(char **args, int input_redirect, int output_redirect, char *input_file, char *output_file);
char **parse_command(char *line, int *input_redirect, int *output_redirect, char **input_file, char **output_file);

void print_prompt() {
    printf("mysh> ");
}

void print_welcome_msg() {
    printf("Welcome to my shell!\n");
}

int is_builtin(char *command) {
    for (int i = 0; i < num_builtins; i++) {
        if (strcmp(command, builtin_commands[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// void execute_builtin(char **args) {
//     if (strcmp(args[0], "cd") == 0) {
//         if (args[1] == NULL) {
//             fprintf(stderr, "cd: expected argument\n");
//         } else {
//             struct stat st;
//             if (stat(args[1], &st) == 0 && S_ISDIR(st.st_mode)) {
//                 if (chdir(args[1]) != 0) {
//                     perror("cd");
//                 }
//             } else {
//                 fprintf(stderr, "cd: %s: Not a directory\n", args[1]);
//             }
//         }
//     } else if (strcmp(args[0], "pwd") == 0) {
//         char cwd[MAX_PATH];
//         if (getcwd(cwd, sizeof(cwd)) != NULL) {
//             printf("%s\n", cwd);
//         } else {
//             perror("pwd");
//         }
//     } else if (strcmp(args[0], "which") == 0) {
//         if (args[1] == NULL) {
//             fprintf(stderr, "which: expected argument\n");
//             return;
//         }
//         if (is_builtin(args[1])) {
//             return;
//         }
//         char *path_env = getenv("PATH");
//         char *path = strtok(path_env, ":");
//         while (path != NULL) {
//             char full_path[MAX_PATH];
//             sprintf(full_path, "%s/%s", path, args[1]);
//             if (access(full_path, X_OK) == 0) {
//                 printf("%s\n", full_path);
//                 return;
//             }
//             path = strtok(NULL, ":");
//         }
//     } else if (strcmp(args[0], "exit") == 0) {
//         if (args[1] != NULL) {
//             printf("%s", args[1]);
//             for (int i = 2; args[i] != NULL; i++) {
//                 printf(" %s", args[i]);
//             }
//             printf("\n");
//         }
//         printf("mysh: exiting\n"); // Print "mysh: exiting" here
//         exit(0); // Exit after printing the message
//     }
// }



void execute_builtin(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: expected argument\n");
        } else if (chdir(args[1]) != 0) {
            perror("cd");
        }
    } else if (strcmp(args[0], "pwd") == 0) {
        char cwd[MAX_PATH];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
    } else if (strcmp(args[0], "which") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "which: expected argument\n");
            return;
        }
        if (is_builtin(args[1])) {
            return;
        }
        char *path_env = getenv("PATH");
        char *path = strtok(path_env, ":");
        while (path != NULL) {
            char full_path[MAX_PATH];
            sprintf(full_path, "%s/%s", path, args[1]);
            if (access(full_path, X_OK) == 0) {
                printf("%s\n", full_path);
                return;
            }
            path = strtok(NULL, ":");
        }
    } else if (strcmp(args[0], "exit") == 0) {
        if (args[1] != NULL) {
            printf("%s", args[1]);
            for (int i = 2; args[i] != NULL; i++) {
                printf(" %s", args[i]);
            }
            printf("\n");
        }
        printf("mysh: exiting\n"); // Print "mysh: exiting" here
        exit(0); // Exit after printing the message
    }
}

int is_path(char *command) {
    return strchr(command, '/') != NULL;
}

char *find_executable(char *command) {
    static char full_path[MAX_PATH];
    char *paths[] = {"/usr/local/bin", "/usr/bin", "/bin"};
    for (int i = 0; i < 3; i++) {
        sprintf(full_path, "%s/%s", paths[i], command);
        if (access(full_path, X_OK) == 0) {
            return full_path;
        }
    }
    return NULL;
}



//char *find_executable(char *command);

//OG  one
/*
void execute_command(char **args, int input_redirect, int output_redirect, char *input_file, char *output_file) {
    if (args[0] == NULL) {
        return;
    }

    if (is_builtin(args[0])) {
        execute_builtin(args);
    } else {
        FILE *file = fopen(args[0], "r");
        if (file != NULL) {
            // Execute script file
            fclose(file);
            // Implement script execution logic here
        } else {
            char *executable = find_executable(args[0]);
            if (executable != NULL) {
                pid_t pid = fork();
                if (pid == -1) {
                    perror("fork");
                    return;
                } else if (pid == 0) {
                    // Child process
                    if (input_redirect) {
                        int fd = open(input_file, O_RDONLY);
                        if (fd == -1) {
                            perror("open");
                            exit(1);
                        }
                        dup2(fd, STDIN_FILENO);
                        close(fd);
                    }
                    if (output_redirect) {
                        int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                        if (fd == -1) {
                            perror("open");
                            exit(1);
                        }
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                    }
                    execv(executable, args);
                    perror("execv");
                    exit(1);
                } else {
                    // Parent process
                    int status;
                    waitpid(pid, &status, 0);
                }
            } else {
                fprintf(stderr, "%s: command not found\n", args[0]);
            }
        }
    }
}
*/
//old one
void execute_script(char *script_file) {
    FILE *file = fopen(script_file, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    char line[MAX_PATH];
    while (fgets(line, sizeof(line), file) != NULL) {
        int input_redirect = 0, output_redirect = 0;
        char *input_file = NULL, *output_file = NULL;
        char **script_args = parse_command(line, &input_redirect, &output_redirect, &input_file, &output_file);
        execute_command(script_args, input_redirect, output_redirect, input_file, output_file);
    }

    fclose(file);
}



void execute_command(char **args, int input_redirect, int output_redirect, char *input_file, char *output_file) {
    if (args[0] == NULL) {
        return;
    }

    if (is_builtin(args[0])) {
        execute_builtin(args);
    } else {
        FILE *file = fopen(args[0], "r");
        if (file != NULL) {
            char first_line[MAX_PATH];
            if (fgets(first_line, sizeof(first_line), file) != NULL) {
                if (first_line[0] == '#' && first_line[1] == '!') {
                    // It's a script file, execute it
                    fclose(file);
                    execute_script(args[0]);
                    return;
                }
            }
            fclose(file);
        }

        char *executable = find_executable(args[0]);
        if (executable != NULL) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                return;
            } else if (pid == 0) {
                // Child process
                if (input_redirect) {
                    int fd = open(input_file, O_RDONLY);
                    if (fd == -1) {
                        perror("open");
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }
                if (output_redirect) {
                    int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                    if (fd == -1) {
                        perror("open");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
                execv(executable, args);
                perror("execv");
                exit(1);
            } else {
                // Parent process
                int status;
                waitpid(pid, &status, 0);
            }
        } else {
            fprintf(stderr, "%s: command not found\n", args[0]);
        }
    }
}





// void execute_command(char **args, int input_redirect, int output_redirect, char *input_file, char *output_file) {
//     pid_t pid = fork();
//     if (pid == -1) {
//         perror("fork");
//         return;
//     } else if (pid == 0) {
//         // Child process
//         if (input_redirect) {
//             int fd = open(input_file, O_RDONLY);
//             if (fd == -1) {
//                 perror("open");
//                 exit(1);
//             }
//             dup2(fd, STDIN_FILENO);
//             close(fd);
//         }
//         if (output_redirect) {
//             int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);
//             if (fd == -1) {
//                 perror("open");
//                 exit(1);
//             }
//             dup2(fd, STDOUT_FILENO);
//             close(fd);
//         }
//         if (is_builtin(args[0])) {
//             execute_builtin(args);
//             exit(0);
//         } else if (is_path(args[0])) {
//             execv(args[0], args);
//         } else {
//             char *executable = find_executable(args[0]);
//             if (executable != NULL) {
//                 execv(executable, args);
//             } else {
//                 fprintf(stderr, "%s: command not found\n", args[0]);
//                 exit(1);
//             }
//         }
//         perror("execv");
//         exit(1);
//     } else {
//         // Parent process
//         int status;
//         waitpid(pid, &status, 0);
//     }
// }

/*
oid execute_command(char **args, int input_redirect, int output_redirect, char *input_file, char *output_file) {
    if (args[0] == NULL) {
        return;
    }

    if (is_builtin(args[0])) {
        execute_builtin(args);
    } else if (is_path(args[0])) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return;
        } else if (pid == 0) {
            // Child process
            if (input_redirect) {
                int fd = open(input_file, O_RDONLY);
                if (fd == -1) {
                    perror("open");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            if (output_redirect) {
                int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                if (fd == -1) {
                    perror("open");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            execv(args[0], args);
            perror("execv");
            exit(1);
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        }
    } else {
        char *executable = find_executable(args[0]);
        if (executable != NULL) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                return;
            } else if (pid == 0) {
                // Child process
                if (input_redirect) {
                    int fd = open(input_file, O_RDONLY);
                    if (fd == -1) {
                        perror("open");
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }
                if (output_redirect) {
                    int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                    if (fd == -1) {
                        perror("open");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
                execv(executable, args);
                perror("execv");
                exit(1);
            } else {
                // Parent process
                int status;
                waitpid(pid, &status, 0);
            }
        } else {
            fprintf(stderr, "%s: command not found\n", args[0]);
        }
    }
}
*/

char **parse_command(char *line, int *input_redirect, int *output_redirect, char **input_file, char **output_file) {
    static char *args[MAX_ARGS];
    int arg_count = 0;
    char *token, *quoted_token;
    int in_quotes = 0;

    for (token = strtok(line, " \t\n"); token != NULL; token = strtok(NULL, " \t\n")) {
        quoted_token = token;

        if (*token == '\'' || *token == '"') {
            in_quotes = *token;
            quoted_token++;
        }

        if (in_quotes && *(quoted_token + strlen(quoted_token) - 1) == in_quotes) {
            *(quoted_token + strlen(quoted_token) - 1) = '\0';
            in_quotes = 0;
        }

        if (strcmp(quoted_token, "<") == 0) {
            *input_redirect = 1;
            token = strtok(NULL, " \t\n");
            *input_file = token;
        } else if (strcmp(quoted_token, ">") == 0) {
            *output_redirect = 1;
            token = strtok(NULL, " \t\n");
            *output_file = token;
        } else {
            args[arg_count++] = quoted_token;
        }
    }

    args[arg_count] = NULL;
    return args;
}

// int main(int argc, char **argv) {
//     char line[MAX_PATH];
//     int interactive = isatty(STDIN_FILENO);
//     int should_run = 1;

//     if (interactive) {
//         print_welcome_msg();
//     }

//     while (should_run) {
//         if (interactive) {
//             print_prompt();
//         }

//         if (fgets(line, sizeof(line), stdin) == NULL) {
//             break;
//         }

//         int input_redirect = 0, output_redirect = 0;
//         char *input_file = NULL, *output_file = NULL;
//         char **args = parse_command(line, &input_redirect, &output_redirect, &input_file, &output_file);

//         if (args[0] != NULL) {
//             if (strcmp(args[0], "exit") == 0) {
//                 should_run = 0;
//                 execute_builtin(args);
//             } else {
//                 execute_command(args, input_redirect, output_redirect, input_file, output_file);
//             }
//         }
//     }

//     return 0;
// }

int main(int argc, char **argv) {
    int interactive = isatty(STDIN_FILENO);
    int should_run = 1;

    // Check if a script file is passed as an argument
    if (argc > 1) {
        // Execute the script file
        execute_script(argv[1]);
        return 0; // End the program after script execution
    }

    // If no script file is provided, enter interactive mode
    if (interactive) {
        print_welcome_msg();
    }

    while (should_run) {
        if (interactive) {
            print_prompt();
        }

        char line[MAX_PATH];
        if (fgets(line, sizeof(line), stdin) == NULL) {
            // Break if fgets fails (e.g., if EOF is encountered)
            break;
        }

        // Variables to handle I/O redirection
        int input_redirect = 0, output_redirect = 0;
        char *input_file = NULL, *output_file = NULL;

        // Parse the command line input
        char **args = parse_command(line, &input_redirect, &output_redirect, &input_file, &output_file);

        // Execute the command
        if (args[0] != NULL) {
            if (strcmp(args[0], "exit") == 0) {
                // If the command is exit, set the flag to 0 to break the loop
                should_run = 0;
                execute_builtin(args); // Execute the builtin exit command
            } else {
                // If it's not exit, execute the given command
                execute_command(args, input_redirect, output_redirect, input_file, output_file);
            }
        }
    }

    return 0;
}
