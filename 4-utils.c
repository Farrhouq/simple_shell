#include "shell.h"
/**
 * run_from_file-runs a set of commands from given file
 * @filename:the name of the file
 * @av: the caller's args
 * @env: a set of command from a file
 */
int run_from_file(char *filename, char **av, char **env)
{
	FILE *file = fopen(filename, "r");
	char line[MAX_CMD_LEN];
	int i = 0, ex = 0;

	(void) env;

	if (file == NULL)
	{
		fprintf(stderr, "Failed to open the file %s\n", filename);
		return (1);
	}
	while (fgets(line, sizeof(line), file) != NULL)
	{
		run_semis(line, &i, av, &ex, env);
	}

	fclose(file);

	return (ex);
}
