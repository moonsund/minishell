#include "minishell.h"

void reset_iteration(t_shell *shell);
void shell_destroy(t_shell *shell);
void free_tokens(t_token_list *list);
void free_pipeline(t_pipeline *pl);
void free_env_var_list(t_env_var_list *vars);
void err_print(t_exit_status type, const char *ctx);

void reset_iteration(t_shell *shell)
{
	free_tokens(&shell->tokens);
	free_pipeline(shell->pipeline);
	shell->pipeline = NULL;
}

void shell_destroy(t_shell *shell)
{
	reset_iteration(shell);
	free_env_var_list(&shell->env_vars);
	rl_clear_history();
}

void free_tokens(t_token_list *list)
{
	t_token *cur;
	t_token *next;

	if (!list)
		return ;

	cur = list->head;
	while (cur != NULL)
	{
		next = cur->next;
		free(cur->raw_str);
		free(cur->quotes_map);
		free(cur);
		cur = next;
	}
	list->head = NULL;
	list->tail = NULL;
	list->count = 0;
}

void free_pipeline(t_pipeline *pl)
{
    size_t i;

    if (!pl)
        return;
    
    i = 0;
    while (i < pl->count)
    {
        free_cmd(&pl->cmds[i]);
        i++;
    }
    free(pl->cmds);
    free(pl);
}

void free_env_var_list(t_env_var_list *vars)
{
    t_var *cur;
	t_var *next;

	if (!vars)
		return ;

	cur = vars->head;
	while (cur != NULL)
	{
		next = cur->next;
		free(cur->name);
		free(cur->value);
        free(cur);
		cur = next;
	}
	vars->head = NULL;
    vars->tail = NULL;
	vars->count = 0;
}

void err_print(t_exit_status type, const char *where)
{
	if (type == ES_GENERAL)
		printf("minishell: %s: \n", where);
	else if (type == ES_SYNTAX)
		printf("minishell: syntax_error: %s\n", where);
	else
        printf( "minishell: %s\n", where);
}

void err_malloc_print(const char *where)
{
	printf("minishell: %s: cannot allocate memory\n", where);
}