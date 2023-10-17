#include "shell.h"

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
 * @exit_status: exit status
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
    {
        print_error(name, i, args[0], "not found");
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

int run_semis(char *lineptr, int *i, char **av, int *exit_status, char **env)
{
    char **commands, *line, **args;
    int j, l, run_and_ex;

    if (!(is_spaces(lineptr)) && strlen(lineptr))
    {
        commands = gen_tokens(lineptr, ";");
        j = 0;
        while (commands[j] != NULL)
        {
            line = commands[j++];

            for (l = 0; line[l] != '\0'; l++)
            {
                if (line[l] == '\n' && !line[l + 1])
                    line[l] = '\0';
            }

            if (contains_and_or(line, '&'))
            {
                run_and_ex = run_and(line, av[0], env, i, exit_status);
                if (run_and_ex == -1)
                {
                    free_array(commands);
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

int run_and(char *line, char *name, char **env, int *cmd_count, int *ex)
{
    char **cmds = NULL, *line_c;
    int i = 0, error = 0;
    char **args = NULL;

    cmds = tokenize_and_or(line, '&');
    while (cmds[i] != NULL && !error)
    {
        line_c = cmds[i++];
        args = gen_tokens(line_c, " ");
        if (args == NULL)
            continue;
        /*******/
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
        /******/
        (*cmd_count)++;
        error = run_ret(args, name, env, *cmd_count);
        free_array(args);
    }
    free_array(cmds);
    return (error);
}

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

/*
    Now I have to write functions to check if it contains stuff.
    So first, in the scolon function, we'll take the command, break it down.
    Then we'll take the commands one by one, and for each, we'll take it,
    ask if it contains stuff (generally). If it doesn't, we'll run and move
    to the next. If it does, (duhhh...), then we'll take that command, and pass
    it to the run_and command. So the run_semis function is done here. No more
    nesting.
    Then in the run_and command, we have a line. So we break it down, based on &&.
    If it doesn't contain &&, we'll ask, does it contain stuff(|| at this point?
    If it's a clear line, then it won't even get to this function. So it must have
    something. So at this point, we'll check if it has && first. If yes, then
    we'll break it down, then for each, we'll RUN UNTIL WE FINISH OR GET AN ERROR.
    If no, then we'll just pass it to the run_or function. (Because it surely
    has something in it).
    But let's come back here. If yes, then while not error encountered:
    first we'll check if it's clean. If yes then we'll run and keep track
    of the exit status. If no, then we'll pass it to the run_or function while
    still keeping track of the exit status. Easy, no nesting.
    Now for the run_or function. We'll RUN UNTIL WE DON'T GET AN ERROR. Then
    we'll return 0;


int main()
{
    char *input1 = "example && test";
    char *input2 = "example &&test";
    char *input3 = "example || test";
    char *input4 = "example ||f";

    printf("Input 1: %d\n", contains_and_or(input1, '&')); // Should print 1
    printf("Input 2: %d\n", contains_and_or(input2, '&')); // Should print 0
    printf("Input 3: %d\n", contains_and_or(input3, '|')); // Should print 1
    printf("Input 4: %d\n", contains_and_or(input4, '|')); // Should print 0
}
*/
