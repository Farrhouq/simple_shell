:include "shell.h"
/**
 * gen_tokens - tokenizes the given string with respect to the given delimiter
 * @str: the given string
 * @delim: the delimiter
 *
 * Return: a NULL terminated array of strings
 */
char **gen_tokens(char *str, const char *delim)
{
	int i = 0;
	char *token, *s = str, **tokens = NULL;

	if (str == NULL || delim == NULL)
		return (NULL);
	tokens = malloc(sizeof(char *) * (MAX_ARGS + 1));
	if (tokens == NULL)
	{
		perror("malloc");
		exit(1);
	}
	token = _strtok(s, delim);
	while (token != NULL && i < MAX_ARGS - 1)
	{
		if (token == NULL)
			return (NULL);
		tokens[i] = strdup(token);
		token = _strtok(NULL, delim);
		i++;
	}
	tokens[i] = NULL;

	return (tokens);
}

/**
 * free_array - frees an array
 * @array: the array
 *
 * Return: Always 0
 */
void free_array(char **array)
{
	int j = 0;

	while (array[j] != NULL)
		free(array[j++]);
	free(array);
}

/**
 * _unsetenv - our unsetenv function
 * @envname: the name of the environment varible
 * @env: the caller's environment
 *
 * Return: 0 on success, -1 otherwise
 */

int _unsetenv(char *envname, char **env)
{
	int i = 0, j = 0, found = 0;

	if (envname == NULL)
		return (-1);
	while (env[i] != NULL)
	{
		if (strncmp(env[i], envname, strlen(envname)) == 0)
		{
			found = 1;
			break;
		}
		i++;
	}
	if (!found)
		return (0);
	for (j = i; env[j] != NULL; j++)
		env[j] = env[j + 1];
	return (0);
}

/**
 * _getenv - custom getenv function
 * @var: the environment variable
 * @env: the caller's environment
 *
 * Return: the value of the environment variable or NULL
 */
char *_getenv(char *var, char **env)
{
	int i = 0;
	char *key, *value, *temp, *res;

	if (var == NULL)
		return (NULL);
	while (env[i] != NULL)
	{
		temp = strdup(env[i]);
		key = strtok(temp, "=");
		value = strtok(NULL, "=");
		if (strcmp(key, var) == 0)
		{
			if (strlen(value) < 1)
				return (NULL);
			res = strdup(value);
			free(temp);
			return (res);
		}
		i++;
		free(temp);
	}
	return (NULL);
}

/**
 * _setenv - setenv
 * @var: name of the env
 * @value: value to be set
 * @overwrite:  overwrite?
 * @env: the caller's environment
 *
 * Return: 0 on success, -1 otherwise
 */
int _setenv(char *var, char *value, int overwrite, char **env)
{
	int i = 0, found = 0;
	char *envi;

	if (var == NULL || value == NULL)
		return (-1);
	while (env[i] != NULL)
	{
		if (strncmp(env[i], var, strlen(var)) == 0)
		{
			found = 1;
			break;
		}
		i++;
	}
	if (found && !overwrite)
		return (0);
	envi = malloc(sizeof(char) * (strlen(var) + strlen(value) + 2));
	env[i] = malloc(sizeof(envi));
	strcpy(envi, var);
	strcat(envi, "=");
	strcat(envi, value);
	strcpy(env[i], envi);
	free(envi);
	if (!found)
		env[i + 1] = NULL;
	return (0);
}
