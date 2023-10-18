#include "shell.h"

/**
 * env_command - prints the environment variables
 * @av: the commands
 * @env: the environment of the caller
 *
 * Return: Always 0
 */
int env_command(char **av, char **env)
{
	int j = 0;
	char *temp;

	(void)av;
	while (env[j] != NULL)
	{
		temp = strdup(env[j++]);
		_puts(temp);
		_putchar('\n');
		free(temp);
	}
	return (0);
}

/**
 * handle_setenv - custom setenv command
 * @av: the arguments
 * @env: the caller's environment
 *
 * Return: 0 on success, -1 on error
 */
int handle_setenv(char **av, char **env)
{
	if (av[1] == NULL || av[2] == NULL || av[3] != NULL)
		return (-1);
	return (_setenv(av[1], av[2], 1, env));
}

/**
 * handle_unsetenv - unsetenv command handler
 * @av: the command arguments
 * @env: the caller's environment
 *
 * Return: 0 on success, -1 otherwise
 */
int handle_unsetenv(char **av, char **env)
{
	if (av[1] == NULL || av[2] != NULL)
		return (-1);
	return (_unsetenv(av[1], env));
}

/**
 * cd - change directory
 * @args: the arguments
 * @env: the caller's environment
 *
 * Return: 0 on success, -1 otherwise
 */
int cd(char **args, char **env)
{
	char *old_pwd = malloc(sizeof(char) * (PATH_MAX_LEN + 1));

	old_pwd = _getenv("PWD", env);
	_setenv("OLDPWD", old_pwd, 1, env);
	if (args[1] == NULL)
	{
		if (chdir(_getenv("HOME", env)) == -1)
		{
			free(old_pwd);
			return (-1);
		}
		_setenv("PWD", _getenv("HOME", env), 1, env);
	}
	else if (args[1][0] == '-')
	{
		if (chdir(_getenv("OLDPWD", env)) == -1)
		{
			free(old_pwd);
			return (-1);
		}
		_setenv("PWD", _getenv("OLDPWD", env), 1, env);
	}
	else
	{
		if (chdir(args[1]) == -1)
		{
			free(old_pwd);
			return (-1);
		}
		_setenv("PWD", _getenv(args[1], env), 1, env);
	}
	free(old_pwd);
	return (0);
}
