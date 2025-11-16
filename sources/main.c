#include "minishell.h"

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
            ft_printf("exit\n");
            break;
        }

        if (!is_empty(line))
            add_history(line);

        shell.normalized_cmd_str = normalize_str(line);
        if (!shell.normalized_cmd_str)
            perror("normalize_str");
        ft_printf("[normalize_debug]: \"%s\"\n", shell.normalized_cmd_str);

        // tokenize(line);
        free(line);

        // parser();
        // execute();
    }

    return (EXIT_SUCCESS);
}
