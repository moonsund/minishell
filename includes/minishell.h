#ifndef MINISHELL_H
# define MINISHELL_H

#include "../libft/libft.h"
#include <stdlib.h>		// EXIT_FAILURE, EXIT_SUCCESS
#include <limits.h>		// INT_MAX
#include <sys/time.h>	// time
#include <unistd.h>		// close, pipe, fork, read, write, getcwd, chdir
#include <stdio.h>		// perror
#include <errno.h>		// errno
#include <sys/wait.h>	// waitpid
#include <stdbool.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

typedef enum e_token_type
{
    TOK_WORD,
    TOK_PIPE,           // |
    TOK_REDIR_IN,       // <
    TOK_HEREDOC,        // >
    TOK_REDIR_OUT,      // <<
    TOK_APPEND,         // >>
} t_token_type;

typedef enum e_qmark
{
    Q_NONE, // outside quotes
    Q_SQ,   // inside single quotes '...'
    Q_DQ,   // inside double quotes "..."
}   t_qmark;

typedef struct s_buf
{
    char *characters; // array of accumulated characters
    t_qmark *quotes_map; // array of qmarks for everysingle character in the characters array 
    size_t capacity; // actual array length
    size_t used_length; // array length in use
} t_buf;

typedef struct s_lexer_context
{
    size_t i;
    bool in_sq;
    bool in_dq;
    t_qmark quote_mark;
    t_buf buf;
    bool malloc_error;
} t_lexer_context;

typedef struct s_token
{
    char *raw_str;  // '0\'-terminated string
    t_token_type type; // WORD, PIPE, REDIR_IN, etc.
    t_qmark *quotes_map; // array of qmarks for everysingle character in the raw_str
    size_t length; // *raw_str length
    struct s_token  *next;
} t_token;

typedef struct s_token_list
{
    t_token *head;
    t_token *tail;
    size_t count;
} t_token_list;

typedef struct s_command
{
    char *command_name;
    char **argv;
    size_t argc;
    struct s_command *next;
} t_command;

typedef struct s_pipeline
{
    t_command *head;
    size_t count;
} t_pipeline;

typedef struct s_shell
{
	int		exit_status;
    char *normalized_cmd_str;
	char	**command_array;
    t_token_list tokens;
    t_pipeline *pipeline; // Abstract Syntax Tree
}	t_shell;


// lexer.c
bool tokenize_with_qmap(const char *str, t_token_list *tokens);
t_token *make_word_token(t_buf *buf);

// lexer_words.c
int process_word_token(t_token_list *tokens, t_lexer_context *ctx);

// lexer_operators.c
int check_operators(const char *str, t_token_list *tokens, t_lexer_context *context);

// lexer_chars.c
int append_char(t_buf *buf, char c, t_qmark quote_mark);

// lexer_utils.c
bool is_empty(const char *str);
bool is_space(unsigned char c);
bool is_operator(unsigned char c);
void append_token(t_token_list *list, t_token *token);
void reset_buf(t_buf *buf);
void free_buf(t_buf *buf);


// utils.c
void free_tokens(t_token_list *list);

// signals.c
void setup_signals(void);


// exec functions
void	execute_commands(t_shell command/*, TBD */);
void	execute_pwd();
void	execute_cd(char *requested_path);

#endif
