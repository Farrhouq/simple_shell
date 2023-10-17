#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>

void run_command(char **args, char *name, char **env, int i, int *ex)
{
	pid_t child_pid;
	char *path;
	int b = 1, status;

	path = find_exec_path(args[0], env);
	if (path == NULL)
	{
		print_error(name, i, args[0], "not found");
		*ex = 127;
		return;
	}
	child_pid = fork();
	if (child_pid == -1)
		print_error(name, i, "fork", "Can't create another process");
	else if (child_pid == 0)
	{
		if (execve(path, args, env) == -1)
			perror(args[0]);
	}
	else
	{
		if (wait(&status) == -1)
			perror("wait");
		else if (WIFEXITED(status))
			*ex = WEXITSTATUS(status);
	}
	free(path);
}


int main(int ac, char **av, char **env)
{
    int i = 1, ex = 0;
	size_t len = 0;
	ssize_t rd;
	char *line = NULL, **tokens = NULL, *prompt = "$ ";

	if (isatty(STDIN_FILENO) == 1)
		_puts(prompt);
	while ((rd = getline(&line, &len, stdin)) != -1)
	{
		if (strlen(line) > 1 && check_spaces(line) != 0)
		{
			remove_newline(line);
			tokens = tokenizer(line, " ");
			if (tokens == NULL)
				continue;
			
			run_command(tokens, av[0], env, i, &ex);
		}
		i++;
		if (isatty(STDIN_FILENO) == 1)
			puts(prompt);
	}
	if (rd == -1 && isatty(STDIN_FILENO) == 1)
		putchar('\n');
	free(line);
    
	return (ex);
}

/**
 * find_exec_path - finds the path of a command
 * @command: the command
 *
 * Return: the full path if found else NULL
 */
char *find_exec_path(char *command, char **env)
{
	char *complete_path = NULL, *path = NULL, **dirs = NULL;
	char *colon = ":";
	int i = 0;

	if (command == NULL)
		return (NULL);
	if (access(command, X_OK) == 0)
		return (strdup(command));

	path = _getenv("PATH", env);
	if (path == NULL)
		return (NULL);
	dirs = gen_tokens(path, colon);
	while (dirs[i] != NULL)
	{
		complete_path = construct_path(command, dirs[i]);
		if (access(complete_path, X_OK) == 0)
		{
			free(path);
			return (complete_path);
		}
		i++;
		if (dirs[i] != NULL)
			free(complete_path);
	}
	free(complete_path);
	free(path);
	return (NULL);
}