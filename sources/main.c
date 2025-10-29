#include "minishell.h"

static void print_token_list(t_token_list *list);

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
    shell.list.head = NULL;
    shell.list.count = 0;
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
        printf("[readline_debug]: \"%s\"\n", line);

        shell.normalized_cmd_str = normalize_str(line);
        if (!shell.normalized_cmd_str)
            perror("normalize_str");
        free(line);
        printf("[normalize_debug]: \"%s\"\n", shell.normalized_cmd_str);

        tokenize(shell.normalized_cmd_str, &shell.list);
        print_token_list(&shell.list);

        
        // parser();
        // execute();
    }

    return (EXIT_SUCCESS);
}


static void print_token_list(t_token_list *list)
{
    t_token *cur;

    cur = list->head;
    while (cur != NULL)
    {
        printf("[debug list token] text: %s, type: %u\n", cur->text, cur->type);
        cur = cur->next;
    }
}