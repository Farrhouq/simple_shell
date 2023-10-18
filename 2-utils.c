#include "shell.h"

/**
 * contains_and_or - checks if a command line has logical operators || or &&
 * @line: the line
 * @and_or: specifier & or |
 *
 * Return: 1 if true, else 0
*/
int contains_and_or(char *line, char and_or)
{
	int i = 0;
	char spec = and_or;

	while (line[i] != '\0')
	{
		if (line[i] == spec)
		{
			if (line[i + 1] == spec)
			{
				if (line[i + 2] != '\0')
					return (1);
			}
		}
		i++;
	}
	return (0);
}

/**
 * run_ret - runs the command
 * @args: the commands
 * @name: the name of the caller
 * @env: environment variables of the caller
 * @i: the command count
 *
 * Return: the exit status of the command
 */

int run_ret(char **args, char *name, char **env, int i)
{
	pid_t child_pid;
	char *path;
	int b = 1, status, ex;

	b = run_built_in(args, env);
	if (b == 0)
		return (b);
	else if (b == -1)
	{
		print_error(name, i, args[0], "error");
		return (b);
	}
	path = find_exec_path(args[0], env);
	if (path == NULL)
	{   print_error(name, i, args[0], "not found");
		ex = 127;
		free(path);
		return (ex);
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
		{
			ex = WEXITSTATUS(status);
			free(path);
			return (ex);
		}
	}
	free(path);
	return (0);
}

/**
 * run_semis - handles command separator ';'
 * @lineptr: the command line
 * @i: the command count - for errors
 * @av: the caller's args
 * @exit_status: exit status of the caller to be set here
 * @env: the caller's environment
 *
 * Return: 1 if exiting else 0
 */
int run_semis(char *lineptr, int *i, char **av, int *exit_status, char **env)
{
	char **commands, *line, **args;
	int j, run_and_ex;

	if (!(is_spaces(lineptr)) && strlen(lineptr))
	{
		commands = gen_tokens(lineptr, ";");
		j = 0;
		while (commands[j] != NULL)
		{   line = commands[j++];
			clean_line(line);
			replace_special_env(line, *exit_status);
			replace_env_vars(line, env);
			if (contains_and_or(line, '&') || contains_and_or(line, '|'))
			{   run_and_ex = run_and(line, av[0], env, i, exit_status);
				if (run_and_ex == -1)
				{   free_array(commands);
					return (1);
				}
				continue;
			}
			args = gen_tokens(line, " ");
			if (args == NULL)
				continue;
			if (strcmp(args[0], "exit") == 0)
			{
				if (args[1] != NULL)
					*exit_status = atoi(args[1]);
				free_array(args);
				free_array(commands);
				return (1);
			}
			(*i)++;
			run_command(args, av[0], env, *i, exit_status);
			free_array(args);
		}
	}
	free_array(commands);
	return (0);
}

/**
 * run_and - handles the logical operator &&
 * @line: the command line
 * @name: the name of the caller
 * @env: the caller's environment
 * @cmd_count: the command count - for errors
 * @ex: exit status of the caller to be set here
 *
 * Return: -1 if exiting else 0
 */
int run_and(char *line, char *name, char **env, int *cmd_count, int *ex)
{
	char **cmds = NULL, *line_c;
	int i = 0, error = 0, run_or_ex;
	char **args = NULL;

	cmds = tokenize_and_or(line, '&');
	while (cmds[i] != NULL && !error)
	{   line_c = cmds[i++];
		if (contains_and_or(line_c, '|'))
		{   run_or_ex = run_or(line_c, name, env, cmd_count, ex);
			if (run_or_ex == -1)
			{   free_array(cmds);
				return (-1);
			}
			continue;
		}

		args = gen_tokens(line_c, " ");
		if (args == NULL)
			continue;
		if (strcmp(args[0], "exit") == 0)
		{
			if (args[1] != NULL)
			{
				*ex = atoi(args[1]);
				free_array(args);
				free_array(cmds);
				return (-1);
			}
			*ex = 0;
			free_array(args);
			free_array(cmds);
			return (-1);
		}
		(*cmd_count)++;
		error = run_ret(args, name, env, *cmd_count);
		free_array(args);
	}
	free_array(cmds);
	return (error);
}

/**
 * run_or - handles the logical operator ||
 * @line: the command line
 * @name: the name of the caller
 * @env: the caller's environment
 * @cmd_count: the command count - for errors
 * @ex: exit status of the caller to be set here
 *
 * Return: -1 if exiting else 0
 */
int run_or(char *line, char *name, char **env, int *cmd_count, int *ex)
{
	char **cmds = NULL, *line_c;
	int i = 0, error = 1;
	char **args = NULL;

	cmds = tokenize_and_or(line, '|');
	while (cmds[i] != NULL && error)
	{
		line_c = cmds[i++];

		args = gen_tokens(line_c, " ");
		if (args == NULL)
			continue;
		if (strcmp(args[0], "exit") == 0)
		{
			if (args[1] != NULL)
			{
				*ex = atoi(args[1]);
				free_array(args);
				free_array(cmds);
				return (-1);
			}
			*ex = 0;
			free_array(args);
			free_array(cmds);
			return (-1);
		}
		(*cmd_count)++;
		error = run_ret(args, name, env, *cmd_count);
		free_array(args);
	}
	free_array(cmds);
	return (error);
}
