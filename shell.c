#include "shell.h"
/**
 * main - a simple shell
 * @ac: argument count
 * @av: array of commandline arguments
 * @env: the environment variables
 *
 * Return: 0 on success, 1 otherwise
 */
int main(__attribute__((unused)) int ac, char **av, char **env)
{
	int cmd_count = 0, exit_status = 0, brk = 0;
	ssize_t read;
	size_t len = 0;
	char *lineptr = NULL;
	char **aliases;

	if (av[1] != NULL)
	{
		printf("executing from file %s\n", av[1]);
		exit_status = run_from_file(av[1], av, env);
		return (exit_status);
	}

	(void)aliases;
	putprompt();
	while (!brk && ((read = _getline(&lineptr, &len, stdin)) != -1))
	{
		brk = run_semis(lineptr, &cmd_count, av, &exit_status, env);
		putprompt();
	}
	if (read == -1 && isatty(STDIN_FILENO) == 1)
		_putchar('\n');
	free(lineptr);
	return (exit_status);
}

/**
 * is_spaces - checks if a newline terminated string is all spaces
 * @str: the string
 *
 * Return: 1 if true otherwise 0
 */
int is_spaces(char *str)
{
	unsigned int i = 0;
	char nt = '\0', tab = '\t', nl = '\n', space = ' ';

	while (str[i] != nl && str[i] != nt)
	{
		if (str[i] != space && str[i] != tab)
			return (0);
		i++;
	}
	return (1);
}

/**
 * find_exec_path - finds the path of a command
 * @command: the command
 * @env:
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

char *construct_path(char *command, char *path)
{
	char *c_path = NULL, *slash = "/";
	int len;

	len = strlen(command) + strlen(path) + 2;
	c_path = malloc((sizeof(char) * len));
	if (c_path == NULL)
		return (NULL);
	strcpy(c_path, path);
	strcat(c_path, slash);
	strcat(c_path, command);

	return (c_path);
}

/**
 * run_command - runs the command
 * @args: the commands
 * @name: the name of the caller
 * @env: environment variables of the caller
 * @i: the command count
 * @exit_status: exit status
 */
void run_command(char **args, char *name, char **env, int i, int *ex)
{
	pid_t child_pid;
	char *path;
	int b = 1, status;

	b = run_built_in(args, env);
	if (b == -1)
	{
		print_error(name, i, args[0], "error");
		return;
	}
	else if (b == 0)
		return;
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

/**
 * run_built_in - runs a built-in command
 * @args: the list of built-in commands
 *
 * Return: 0 on success, 1 otherwise
 */
int run_built_in(char **args, char **env)
{
	int i = 0;

	builtin_t builtins[] = {
		{"setenv", handle_setenv},
		{"unsetenv", handle_unsetenv},
		{"env", env_command},
		{"cd", cd},
		{NULL, NULL}};

	while (builtins[i].cmd != NULL)
	{
		if (strcmp(args[0], builtins[i].cmd) == 0)
		{
			if (builtins[i].func(args, env) == -1)
				return (-1);
			return (0);
		}
		i++;
	}
	return (1);
}

void putprompt(void)
{
	char *prompt = "$ ";
	if (isatty(STDIN_FILENO) == 1)
		_puts(prompt);
}
