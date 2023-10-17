#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>


int main(int ac, char **av, char **env)
{
    int i = 1, ex = 0;
	size_t len = 0;
	ssize_t rd;
	char *line = NULL, **tokens = NULL, *prompt = "$ ";

	if (isatty(STDIN_FILENO) == 1)
		_puts(prompt);
	while ((rd = _getline(&line, &len, stdin)) != -1)
	{
		if (strlen(line) > 1 && check_spaces(line) != 0)
		{
			remove_newline(line);
			tokens = tokenizer(line, " ");
			if (tokens == NULL)
				continue;
			if (strcmp(tokens[0], "exit") == 0)
			{
				if (tokens[1] != NULL)
					ex = atoi(tokens[1]);
				free_tokens(tokens);
				break;
			}
			run_command(tokens, av[0], env, i, &ex);
			free_tokens(tokens);
		}
		i++;
		if (isatty(STDIN_FILENO) == 1)
			_puts(prompt);
	}
	if (rd == -1 && isatty(STDIN_FILENO) == 1)
		_putchar('\n');
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
			free_array(dirs);
			free(path);
			return (complete_path);
		}
		i++;
		if (dirs[i] != NULL)
			free(complete_path);
	}
	free(complete_path);
	free(path);
	free_array(dirs);
	return (NULL);
}