#include "minishell.h"

static int process_operator_token(t_token_type type, const char *literal, t_token_list *tokens, t_lexer_context *context);
static t_token *make_operator_token(t_token_type type, const char *literal);

int check_operators(const char *str, t_token_list *tokens, t_lexer_context *context)
{
    t_token *token;

    if (!context->in_sq && !context->in_dq && is_operator(str[context->i]))
    {
        if (context->buf.used_length > 0)
        {
            token = make_word_token(&context->buf);
            if (!token)
            {
                err_malloc_print("tokenizer: word token");
                free_buf(&context->buf);
                return (-1);
            }
            append_token(tokens, token);
            reset_buf(&context->buf);
        }

        if (str[context->i] == '|') 
            return (process_operator_token(TOK_PIPE, "|", tokens, context));

        else if (str[context->i] == '<')
        {
            if (str[context->i+1] == '<')
                return (process_operator_token(TOK_HEREDOC, "<<", tokens, context));
            else
                return (process_operator_token(TOK_REDIR_IN, "<", tokens, context));
        }

        else if (str[context->i] == '>')
        {
            if (str[context->i + 1] == '>')
                return (process_operator_token(TOK_APPEND, ">>", tokens, context));
            else
                return (process_operator_token(TOK_REDIR_OUT, ">", tokens, context));
        } 
    }
    return (0);
}

static int process_operator_token(t_token_type type, const char *literal, t_token_list *tokens, t_lexer_context *context)
{
    t_token *token;

    token = make_operator_token(type, literal);
    if (!token)
    {
        err_malloc_print("tokenizer: operator token");
        free_buf(&context->buf);
        return (-1);
    }
    append_token(tokens, token);
    context->i += ft_strlen(literal);
    return (1);
}

static t_token *make_operator_token(t_token_type type, const char *literal)
{
    t_token *token;
    size_t len;

    token = malloc(sizeof(*token));
    if (!token)
        return (NULL);

    len = ft_strlen(literal);
    token->raw_str = malloc(sizeof(char) * (len + 1));
    if (!token->raw_str)
    {
        free(token);
        return (NULL);
    }

    ft_memcpy(token->raw_str, literal, len + 1);
    token->length = len;
    token->type = type;
    token->quotes_map = NULL;
    token->next = NULL;
    return (token);
}