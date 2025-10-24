#include "minishell.h"

static bool is_empty(const char *str);

int main(int argc, char **argv, char **envp)
{
    char *line;
    t_shell	shell;

    shell.exit_status = 1;
    (void)argc;
    (void)argv;
    (void)envp;


    setup_signals();
    // init();
    while(true) // or exit_status
    {
        line = readline("minishell> ");
        if (!line)
        {
            printf("exit\n");
            break;
        }

        if (!is_empty(line))
            add_history(line);

        printf("[debug] typed: \"%s\"\n", line);
        free(line);
        // parser();
        // execute();
    }

    return (EXIT_SUCCESS);
}

// will be moved away later
static bool is_empty(const char *str)
{
    if (!str || !*str) return true;
    while (*str) {
        if (!isspace((unsigned char)*str)) return false;
        str++;
    }
    return true;
}