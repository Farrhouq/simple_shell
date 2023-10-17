#include "shell.h"

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