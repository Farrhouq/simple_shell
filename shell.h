#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define PATH_MAX_LEN 100
#define MAX_CMD_LEN 100
#define MAX_ARGS 10
#define BUFF_SIZE 1024

/**
 * struct builtin - struct for the built-in commands
 * @cmd: the command
 * @func: the corresponding function
 */
typedef struct builtin
{
	char *cmd;
	int (*func)(char **, char **);
} builtin_t;

int is_spaces(char *);
char *find_exec_path(char *, char **env);
void run_command(char **, char *, char **, int, int *);
int run_built_in(char **, char **);

int _putchar(char);
int _puts(char *);
char *_getenv(char *, char **env);
ssize_t _getline(char **, size_t *, FILE *);
char *_strtok(char *, const char *);

char **gen_tokens(char *, const char *);
void print_error(char *, int, char *, char *);

int _setenv(char *, char *, int, char **env);
int _unsetenv(char *, char **env);
int cd(char **, char **env);

int env_command(char **, char **);
void free_array(char **);
int handle_setenv(char **, char **);
int handle_unsetenv(char **, char **);
void putprompt(void);
char *construct_path(char *command, char *path);
int run_ret(char **args, char *name, char **env, int i);
int contains_and_or(char *line, char and_or);
int run_semis(char *lineptr, int *i, char **av, int *exit_status, char **env);

int run_and(char *line, char *name, char **env, int *cmd_count, int *ex);
char **tokenize_and_or(char *line, char and_or);

#endif
