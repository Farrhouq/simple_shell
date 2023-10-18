#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * tokenize_and_or - splits a command on logical operators && or ||
 * @line: the command line
 * @and_or: specifier for & or |
 *
 * Return: an array of strings (the commands)
 */
char **tokenize_and_or(char *line, char and_or)
{
	int i = 0;
	char *delim = (and_or == '&') ? "&&" : "||";
	char *token, *s = line, **tokens = NULL;

	if (line == NULL || delim == NULL)
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
 * clean_line - removes comments and trailing whitespace frrom line
 * @line: the command line
 */
void clean_line(char *line)
{
	int i = 0;

	for (i = 0; line[i] != '\0'; i++)
	{
		if ((line[i] == '\n' && !line[i + 1]) || line[i] == '#')
			line[i] = '\0';
	}
}
/**
 * replace_env_vars-
 */
void replace_env_vars(char *line, char **env)
{
	int i = 0;
	int j = 0;
	int lineLength = strlen(line);
	char *modifiedLine = malloc((lineLength + 1) * sizeof(char)), *envVarValue;
	char *envVarName;

	while (i < lineLength)
	{
		if (line[i] == '$')
		{
			int start = i + 1;
			int end = start;

			while (end < lineLength && (isalnum(line[end]) || line[end] == '_'))
				end++;
			if (end > start)
			{
				envVarName = malloc(sizeof(char) * (end - start + 1));
				strncpy(envVarName, line + start, end - start);
				envVarName[end - start] = '\0';
				envVarValue = _getenv(envVarName, env);

				if (envVarValue != NULL)
				{
					strncpy(modifiedLine + j, envVarValue, strlen(envVarValue));
					j += strlen(envVarValue);
				}
				i = end;
			}
		}
		else
		{
			modifiedLine[j] = line[i];
			i++;
			j++;
		}
	}
	modifiedLine[j] = '\0';
	strcpy(line, modifiedLine);
	free(modifiedLine);
}

void replace_special_env(char *line, int exit_status)
{
	int i = 0, j = 0;
	int lineLength = strlen(line);
	char *modifiedLine = malloc((lineLength + 1) * sizeof(char));
	char *exitStatusStr;
	char pidStr[16];

	while (i < lineLength)
	{
		if (line[i] == '$' && line[i + 1] == '$')
		{
			snprintf(pidStr, sizeof(pidStr), "%d", getpid());
			strncpy(modifiedLine + j, pidStr, strlen(pidStr));
			j += strlen(pidStr);
			i += 2;
		}
		else if (line[i] == '$' && line[i + 1] == '?')
		{
			exitStatusStr = int_to_str(exit_status);
			strncpy(modifiedLine + j, exitStatusStr, strlen(exitStatusStr));
			j += strlen(exitStatusStr);
			i += 2;
			free(exitStatusStr);
		}
		else
		{
			modifiedLine[j] = line[i];
			i++;
			j++;
		}
	}

	modifiedLine[j] = '\0';
	strcpy(line, modifiedLine);

	free(modifiedLine);
}

char *int_to_str(int num)
{
	int i = 0;
	int n = num;
	char *str;

	if (num == 0)
	{
		str = malloc(sizeof(char) * 2);
		str[0] = '0';
		str[1] = '\0';
		return (str);
	}

	while (n > 0)
	{
		n /= 10;
		i++;
	}

	n = num;
	str = malloc(sizeof(char) * (i + 1));

	if (str == NULL)
		return (NULL);

	str[i] = '\0';

	while (i > 0)
	{
		str[i - 1] = '0' + (n % 10);
		n /= 10;
		i--;
	}

	return (str);
}
