#ifndef MINISHELL_H
# define MINISHELL_H

#include "libft.h"

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
#include <fcntl.h>		// open
#include <sys/types.h>	// opendir
#include <dirent.h>		// opendir
#include <sys/stat.h>	// open

// Easier Debug
# define NC "\e[0m"
# define BLUE "\e[34m"
# define MAGENTA "\e[35m"
# define YELLOW "\e[33m"
# define RED "\e[31m"
# define GREEN "\e[32m"
# define CYAN "\e[36m"
#ifndef SIGNALS_H
#define SIGNALS_H

extern volatile sig_atomic_t g_sigint;

void setup_signals(void);

#endif

typedef enum e_exit_status
{
	ES_SUCCESS         = 0,   // success
	ES_GENERAL         = 1,   // any “generic” error (including malloc failures, open/dup2 errors in redirections, etc.)
	ES_BUILTIN_MISUSE  = 2,   // incorrect usage of a builtin / invalid builtin arguments
	ES_NOT_EXECUTABLE  = 126, // command or file found, but cannot be executed (EACCES, is a directory, not executable)
	ES_NOT_FOUND       = 127, // command not found (PATH lookup failed / file does not exist)
	ES_SIGINT          = 130, // 128 + SIGINT (2)
	ES_SIGQUIT         = 131, // 128 + SIGQUIT (3)
	ES_SYNTAX          = 258, // parser syntax error
}	t_exit_status;



typedef enum e_token_type
{
	TOK_WORD,
	TOK_PIPE,		// |
	TOK_REDIR_IN,	// <
	TOK_HEREDOC,	// <<
	TOK_REDIR_OUT,	// >
	TOK_APPEND,		// >>
} t_token_type;

typedef enum e_qmark
{
	Q_NONE,	// outside quotes
	Q_SQ,	// inside single quotes '...'
	Q_DQ,	// inside double quotes "..."
}   t_qmark;

typedef struct s_buf
{
	char *characters;		// array of accumulated characters
	t_qmark *quotes_map;	// array of qmarks for everysingle character in the characters array
	size_t capacity;		// actual array length
	size_t used_length;		// array length in use
} t_buf;

typedef struct s_lexer_context
{
	size_t i;
	bool in_sq;
	bool in_dq;
	t_qmark quote_mark;
	t_buf buf;
} t_lexer_context;

typedef struct s_token
{
	char *raw_str;				// '0\'-terminated string
	t_token_type type;			// WORD, PIPE, REDIR_IN, etc.
	t_qmark *quotes_map;		// array of qmarks for everysingle character in the raw_str
	size_t length;				// *raw_str length
	struct s_token  *next;
} t_token;

typedef struct s_token_list
{
	t_token *head;
	t_token *tail;
	size_t count;
} t_token_list;

typedef struct s_env
{
	char			*variable_name;
	char			*variable_data;
	struct s_env	*next;
}	t_env;

typedef struct s_var
{
	char			*name;
	char			*value;
	struct s_var	*next;
}	t_var;

typedef struct s_env_var_list
{
	t_var *head;
	t_var *tail;
	size_t count;
} t_env_var_list;

typedef struct s_command
{
	char **argv;				// null-terminated array of arguments
	char *infile;				// <
	char *outfile;				// > or >>
	int append;					// 0 for > and 1 for >>
	char *heredoc_limiter;
	int has_heredoc;			// 0 or 1
	int heredoc_expand_needed;	// 0 or 1
} t_command;

typedef struct s_pipeline
{
	t_command *cmds;
	size_t count;
} t_pipeline;

typedef struct s_parser_context
{
    t_command current_cmd;
    t_token *current;
    t_token *next;
    int cmd_started;
    char *tmp;
} t_parser_context;


typedef struct s_shell
{
	int				exit_status;
	t_env_var_list	env_vars;				// envp vars saved in linked list
	t_token_list	tokens;
	t_pipeline		*pipeline;				// Only base to consider for exec

	// t_ast ast;

}	t_shell;





// ------------------------------------------------------------------------------------------ From Leo

// main.c

// init.c
int init_shell(t_shell *shell, char **envp);

// envp.c
t_var  *find_var(t_env_var_list *list, const char *name);
int     set_var(t_env_var_list *list, const char *name, const char *value);   // export
int     unset_var(t_env_var_list *list, const char *name);                    // unset
char   *get_var_value(t_env_var_list *var_list, const char *var);                // my_getenv
char  **build_envp(t_env_var_list *list);                                     // for execve

// envp_utils.c
void free_envp_partial(char **envp, size_t used);

// expand.c
t_exit_status expand_tokens(t_token_list *tokens, t_env_var_list *env_vars, t_exit_status exit_status);
char *get_last_status_string(t_exit_status exit_status);

// parcer_init.c
t_pipeline *init_pipeline();
void init_parser_context(t_parser_context *ctx);
void init_command(t_command *cmd);

// parcer_tokens.c
t_exit_status process_tokens(t_pipeline *pl, t_token_list *list, t_parser_context *ctx);

// parcer_utils.c
void free_cmd(t_command *cmd);

// parser.c
t_exit_status  build_pipeline_from_tokens(t_shell *shell);
int append_cmd(t_pipeline *pl, t_command cmd);

// tokenizer.c
t_exit_status  tokenize_with_qmap(const char *str, t_token_list *tokens);
t_token *make_word_token(t_buf *buf);

// tokenizer_words.c
int process_word(t_token_list *tokens, t_lexer_context *ctx);

// tokenizer_operators.c
int check_operators(const char *str, t_token_list *tokens, t_lexer_context *context);

// tokenizer_chars.c
int append_char(char c, t_buf *buf, t_qmark quote_mark);
int boost_buf(t_buf *buf, size_t needed_length);

// tokenizer_utils.c
bool is_empty(const char *str);
bool is_space(unsigned char c);
bool is_operator(unsigned char c);
void append_token(t_token_list *list, t_token *token);
void reset_buf(t_buf *buf);
void free_buf(t_buf *buf);
void init_buffer(t_buf *buf);

// heredoc
int process_heredoc(t_pipeline *pipeline, t_env_var_list *env_vars, int exit_status);

// utils.c
void reset_iteration(t_shell *shell);
void shell_destroy(t_shell *shell);
void free_tokens(t_token_list *list);
void free_pipeline(t_pipeline *pl);
void free_env_var_list(t_env_var_list *vars);
void err_print(t_exit_status exit_status, const char *ctx);
void err_malloc_print(const char *where);

// signals.c
void setup_signals(void);

// ------------------------------------------------------------------------------------------ Exec functions
// pre exec functions
void	check_command_type_and_execute(t_shell *minishell);
void	execute_built_in_commands(t_shell *minishell);
void	execute_external_commands(t_shell *minishell);

// exec built in functions
char	*fetch_current_working_directory(void);
void	execute_echo(t_command *cmds);
void	execute_cd(t_command *cmds);
void	execute_pwd(char *current_working_directory);
// void	execute_export(t_shell *minishell);			// Leo handles
// void	execute_unset(t_shell *minishell);			// Leo handles
// void	execute_env(t_shell *minishell);			// Leo handles
void	execute_exit(t_shell *minishell);

// env related functions -- TO DELETE
// t_env	**build_environment(void);
// t_env	*create_new_environment_variable(char *key, char *value);
// char	*fetch_value_from_key(t_env **head, char *key);
// t_env	*search_last_var(t_env *env_var);
// void	add_env_var_to_list(t_env **head, t_env *new);
// void	delete_env_var(t_env **head, char *var_to_delete);

// exec external functions
char	*build_path(char *file_name);
int		fetch_fd(char *file_name, bool append, bool truncate);
void	fork_and_exec(t_shell *minishell, int *fd, char	**execve_args);

// Free functions
// void	del_string(char *param);
// void	free_all_vars(t_env **head);
#endif
