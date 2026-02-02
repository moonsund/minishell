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
	ES_SUCCESS			= 0,   // success
	ES_GENERAL			= 1,   // any “generic” error (including malloc failures, open/dup2 errors in redirections, etc.)
	ES_INVALID_USAGE	= 2,   // syntax error and incorrect usage of a builtin
	ES_NOT_EXECUTABLE	= 126, // command or file found, but cannot be executed (EACCES, is a directory, not executable)
	ES_NOT_FOUND		= 127, // command not found (PATH lookup failed / file does not exist)
	ES_SIGINT			= 130, // 128 + SIGINT (2)
	ES_SIGQUIT			= 131, // 128 + SIGQUIT (3)
} t_exit_status;

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
} t_qmark;

typedef enum e_redir_type
{
	R_IN,
	R_OUT,
	R_APPEND,
	R_HEREDOC
}	t_redir_type;

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
} t_env;

typedef struct s_var
{
	char			*name;
	char			*value;
	struct s_var	*next;
} t_var;

typedef struct s_env_var_list
{
	t_var *head;
	t_var *tail;
	size_t count;
} t_env_var_list;

typedef struct s_redir
{
	t_redir_type		type;		// R_IN / R_OUT / R_APPEND / R_HEREDOC
	int					fd;			// 0 or 1
	char				*target;	// filename or limiter
	int					expand;		// only for heredoc: 0 or 1
	struct s_redir		*next;
}	t_redir;

typedef struct s_command
{
	char **argv; // null-terminated array of arguments
	// int argc;
	t_redir	*redirs;
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

} t_shell;


// ------------------------------------------------------------------------------------------ From Leo

// main.c

// init.c
int init_shell(t_shell *shell, char **envp);

// envp.c
t_var	*find_var(t_env_var_list *list, const char *name);
int		set_var(t_env_var_list *list, const char *name, const char *value);		// export
int		unset_var(t_env_var_list *list, const char *name);						// unset
char	*get_var_value(t_env_var_list *var_list, const char *var);				// my_getenv
char	**build_envp(t_env_var_list *list);										// before execve
void	sort_envp_alpha(char **envp);											// export no args

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
int			process_heredoc(t_pipeline *pipeline, t_env_var_list *env_vars, t_exit_status exit_status);
static char	*generate_heredoc_filename(size_t heredoc_index);
static int	expand_heredoc(char **line, t_env_var_list *env_vars, t_exit_status exit_status);
int			write_line_in_fd(int fd, char *line);
static int	append_charter(char **line, char c);
static int	append_string(char **line, const char *str);
static void	redir_replace_with_infile(t_redir *r, char *filename);
static int	get_heredoc(t_redir *redir, t_env_var_list *env_vars, t_exit_status exit_status, size_t *heredoc_index);

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

// execution.c
int			execute_pipeline(t_shell *shell);
int			exec_pipeline_forking(t_shell *shell, const t_pipeline *pl);
static void	apply_redirs_or_die(const t_command *cmd);
static int	open_redir_file(const t_redir *redir);
static int	wait_all_and_get_last(pid_t *pids, size_t count);

// exec_command_filter.c
int	is_builtin(const char *cmd);
int	is_parent_builtin(const char *cmd);
int	run_builtin_without_output_in_parent(t_shell *shell, t_command *cmd);
int	run_any_builtin_in_child(t_shell *shell, t_command *cmd);
int	execute_built_in_commands(t_shell *minishell);

// exec_builtin_commands.c
char	*fetch_current_working_directory(void);
int	execute_echo(t_command *cmds);
bool	is_line_return(char **cmd, int *i);
int	execute_cd(t_command *cmds);
int	execute_pwd(char *current_working_directory);
int	execute_exit(t_shell *minishell);

// exec_builtin_commands_env.c
int	execute_export(t_shell *minishell);
int	execute_unset(t_shell *minishell);
int	execute_env(t_shell *minishell);
void	sort_envp_alpha(char **envp);								// export no args
void	str_swap(char **s1, char **s2);

// exec_external_commands.c
char	*build_path_to_check(char *dir, char c, char *cmd);
char	*fetch_and_check_bin_path(t_shell *minishell, char *cmd);
int		execute_external_commands(t_shell *minishell, t_command *cmd);

// exec_utils_fd.c
char	*build_path(char *file_name);
int		open_fd(char *file_name, bool append, bool truncate);
// void	fd_update_if_redirections(t_command *all_commands, int *fd_in, int *fd_out);
void	open_and_close_fd(t_command *cmd);

// exec_close_and_free.c
void	close_and_set_to_neg(int *fd);
void	close_if_valid(int fd);
void	free_envp(t_env_var_list *list);
void	free_strings_array(char **array);

#endif
