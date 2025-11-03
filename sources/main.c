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
    shell.tokens.head = NULL;
    shell.tokens.count = 0;
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

        if (!tokenize(line, &shell.tokens))
        {
            printf("minishell: syntax error: unexpected end of file\n");
            shell.exit_status = 2;
            free(line);
            continue;
        }
        print_token_list(&shell.tokens); // for debugging, to be deleted
        
        // parser();
        // execute();
        clean(&shell.tokens);
    }

    return (EXIT_SUCCESS);
}


static void print_token_list(t_token_list *tokens) // for debugging, to be deleted
{
    static const char *g_token_type_str[] = {
        [WORD]      = "WORD",
        [PIPE]      = "PIPE",
        [REDIR_IN]  = "REDIR_IN",
        [REDIR_OUT] = "REDIR_OUT",
        [APPEND]    = "APPEND",
        [HEREDOC]   = "HEREDOC"
    };

    t_token *cur = tokens->head;
    while (cur)
    {
        const char *type_str = "UNKNOWN";
        if (cur->type >= 0 && cur->type <= HEREDOC)
            type_str = g_token_type_str[cur->type];

        printf("[debug list token] text: %s, type: %s\n",
               cur->text ? cur->text : "(null)",
               type_str);

        cur = cur->next;
    }
}