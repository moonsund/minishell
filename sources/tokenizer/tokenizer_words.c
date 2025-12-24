#include "minishell.h"

t_token *make_word_token(t_buf *buf);

int process_word(t_token_list *tokens, t_lexer_context *ctx)
{
    t_token *token;

    token = make_word_token(&ctx->buf);
    if (!token)
    {
        err_malloc_print("tokenizer: word token");
        free_buf(&ctx->buf);
        return (0);
    }
    append_token(tokens, token);
    reset_buf(&ctx->buf);
    return (1);
}

t_token *make_word_token(t_buf *buf)
{
    t_token *token;

    token = malloc(sizeof(*token));
    if (!token)
        return (NULL);

    token->raw_str = malloc(sizeof(char) * (buf->used_length + 1));
    token->quotes_map = malloc(sizeof(t_qmark) * buf->used_length);
    if (!token->raw_str || !token->quotes_map)
    {
        free(token->raw_str);
        free(token->quotes_map);
        free(token);
        return (NULL);
    }

    if (buf->capacity) 
    {
        ft_memcpy(token->raw_str, buf->characters, buf->used_length);
        ft_memcpy(token->quotes_map, buf->quotes_map,
                buf->used_length * sizeof(*buf->quotes_map));
    }

    token->raw_str[buf->used_length] = '\0';
    token->length = buf->used_length;
    token->type = TOK_WORD;
    token->next = NULL;
    return (token);
}