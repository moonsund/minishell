#include "minishell.h"

// based on Recursive Descent Parser

t_pipeline *parse_command(t_token_list *tokens)
{
    t_pipeline *pipeline;
    t_token *cur;

    pipeline = malloc(sizeof(t_pipeline));
    if (!pipeline)
        return (NULL);
    
    pipeline->count = 0;
    pipeline->head = NULL;

    cur = tokens->head;
    t_command *command;
    command = init_new_command();
    command->command_name = strdup(cur->text);
    cur = cur->next;
        while (cur->type == WORD)
        {
            
            
        }


}

t_command *init_new_command()
{
    t_command *cmd;
    cmd = (t_command *)malloc(sizeof(*cmd));
    if (!cmd) {
        fprintf(stderr, "alloc error: t_command\n");
        return NULL;
    }
    cmd->argc = 0;
    cmd->argv = NULL;
    cmd->command_name = NULL;
    cmd->next = NULL;
    return (cmd);
}