/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: schappuy <schappuy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 20:17:58 by schappuy          #+#    #+#             */
/*   Updated: 2026/02/06 13:04:39 by schappuy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# define _POSIX_C_SOURCE 200809L
# include <stdio.h>		// perror
# include <readline/readline.h>
# include <readline/history.h>
# include "libft.h"
# include <dirent.h>	// opendir
# include <errno.h>		// errno
# include <fcntl.h>		// open
# include <limits.h>	// INT_MAX
# include <signal.h>
# include <stdbool.h>
# include <stdlib.h>	// EXIT_FAILURE, EXIT_SUCCESS
# include <sys/stat.h>	// open
# include <sys/time.h>	// time
# include <sys/types.h>	// opendir
# include <sys/wait.h>	// waitpid
# include <unistd.h>	// close, pipe, fork, read, write, getcwd, chdir

# ifndef SIGNALS_H
#  define SIGNALS_H

extern volatile sig_atomic_t	g_sigint;

void							setup_signals(void);

# endif

typedef enum e_exit_status
{
	ES_SUCCESS = 0,
	ES_GENERAL = 1,
	ES_INVALID_USAGE = 2,
	ES_NOT_EXECUTABLE = 126,
	ES_NOT_FOUND = 127,
	ES_SIGINT = 130,
	ES_SIGQUIT = 131,
}								t_exit_status;

typedef enum e_token_type
{
	TOK_WORD,
	TOK_PIPE,
	TOK_REDIR_IN,
	TOK_HEREDOC,
	TOK_REDIR_OUT,
	TOK_APPEND,
}								t_token_type;

typedef enum e_qmark
{
	Q_NONE,
	Q_SQ,
	Q_DQ,
}								t_qmark;

typedef enum e_redir_type
{
	R_IN,
	R_OUT,
	R_APPEND,
	R_HEREDOC
}								t_redir_type;

typedef struct s_buf
{
	char						*characters;
	t_qmark						*quotes_map;
	size_t						capacity;
	size_t						used_length;
}								t_buf;

typedef struct s_lexer_context
{
	size_t						i;
	bool						in_sq;
	bool						in_dq;
	t_qmark						quote_mark;
	t_buf						buf;
}								t_lexer_context;

typedef struct s_token
{
	char						*raw_str;
	t_token_type				type;
	t_qmark						*quotes_map;
	size_t						length;
	struct s_token				*next;
}								t_token;

typedef struct s_token_list
{
	t_token						*head;
	t_token						*tail;
	size_t						count;
}								t_token_list;

typedef struct s_env
{
	char						*variable_name;
	char						*variable_data;
	struct s_env				*next;
}								t_env;

typedef struct s_strbuf
{
	char						*buf;
	size_t						len;
	size_t						cap;
}								t_strbuf;

typedef struct s_var
{
	char						*name;
	char						*value;
	struct s_var				*next;
}								t_var;

typedef struct s_env_var_list
{
	t_var						*head;
	t_var						*tail;
	size_t						count;
}								t_env_var_list;

typedef struct s_exp_ctx
{
	t_env_var_list				*env;
	t_exit_status				last_status;
}								t_exp_ctx;

typedef struct s_hd_ctx
{
	t_env_var_list				*env;
	t_exit_status				last_status;
	size_t						index;
}								t_hd_ctx;

typedef struct s_redir
{
	t_redir_type				type;
	int							fd;
	char						*target;
	int							expand;
	struct s_redir				*next;
}								t_redir;

typedef struct s_expctx
{
	t_token						*tok;
	t_env_var_list				*env;
	t_exit_status				last;
	t_buf						buf;
}								t_expctx;

typedef struct s_fork_ctx
{
	size_t						i;
	int							prev_read;
	int							pipefds[2];
	pid_t						*pids;
}								t_fork_ctx;

typedef struct s_command
{
	char						**argv;
	t_redir						*redirs;
}								t_command;

typedef struct s_pipeline
{
	t_command					*cmds;
	size_t						count;
}								t_pipeline;

typedef struct s_parser_context
{
	t_command					current_cmd;
	t_token						*current;
	t_token						*next;
	int							cmd_started;
	char						*tmp;
}								t_parser_context;

typedef struct s_shell
{
	int							exit_status;
	int							should_exit;
	char						*input;
	t_env_var_list				env_vars;
	t_token_list				tokens;
	t_pipeline					*pl;
}								t_shell;

// main.c
int								main(int argc, char **argv, char **envp);

// minishell
void							minishell_loop(t_shell *shell);
char							*handle_input(void);
void							process_input(char *input, t_shell *shell);
int								parse_and_prepare(t_shell *shell, char *input);
int								execute_prepared(t_shell *shell);
int								init_shell(t_shell *shell, char **envp);
void							init_env_list_empty(t_env_var_list *list);
char							*find_env_sep(char *env_line);
t_var							*create_var_from_env_line(char *env_line,
									char *sep_ptr);
int								append_var_or_cleanup(t_env_var_list *list,
									t_var *var);
void							reset_iteration(t_shell *shell);
void							shell_destroy(t_shell *shell);
void							free_tokens(t_token_list *list);
void							free_pipeline(t_pipeline *pl);
void							free_env_var_list(t_env_var_list *vars);
void							err_print(t_exit_status exit_status,
									const char *ctx);
void							print_cmd_not_found(const char *cmd);
void							err_malloc_print(const char *where);
void							ctrl_c_handler(int signum);
void							setup_signals(void);
void							ctrl_c_heredoc_handler(int signum);
void							setup_signals_heredoc(void);

// envp.c
t_var							*find_var(t_env_var_list *list,
									const char *name);
int								set_var(t_env_var_list *list, const char *name,
									const char *value);
int								unset_var(t_env_var_list *list,
									const char *name);
char							*get_var_value(t_env_var_list *var_list,
									const char *var);
char							**build_envp(t_env_var_list *list);

// envp_utils.c
void							free_envp_partial(char **envp, size_t used);

// expand.c
t_exit_status					expand_tokens(t_token_list *tokens,
									t_env_var_list *env_vars,
									t_exit_status exit_status);
char							*get_last_status_string(t_exit_status exit_st);
t_exit_status					exp_append_var(t_expctx *c, size_t start,
									size_t end, size_t *i);
size_t							var_end(const char *s, size_t len,
									size_t start);
char							*dup_var_name(const char *s, size_t start,
									size_t end);
t_exit_status					exp_loop(t_expctx *c);
t_exit_status					exp_append_char(t_expctx *c, char ch);
t_exit_status					exp_dollar(t_expctx *c, size_t *i);
t_exit_status					exp_append_status(t_expctx *c, size_t *i);
int								append_str(t_buf *buf, char *str,
									t_qmark quote_mark);

// parser.c
t_exit_status					build_pipeline_from_tokens(t_shell *shell);
int								append_cmd(t_pipeline *pl, t_command cmd);

// parcer_tokens.c
int								process_word_token(t_parser_context *ctx);
t_exit_status					process_pipe_token(t_pipeline *pl,
									t_parser_context *ctx);
t_exit_status					process_redir_tokens(t_parser_context *ctx);
int								append_arg(t_command *cmd, char *arg);

// parcer_init.c
t_pipeline						*init_pipeline(void);
void							init_parser_context(t_parser_context *ctx);
void							init_command(t_command *cmd);
t_redir							*init_redirect(t_parser_context *ctx);

// parcer_helpers.c
int								token_has_any_quotes(t_token *token);
int								redir_push_back(t_redir **lst, t_redir *node);
size_t							count_argv(char **argv);
char							**alloc_argv_with_copy(char **old_argv,
									size_t argc);
int								append_dup_arg(char **new_argv, size_t argc,
									char *arg);

// parcer_utils.c
void							free_cmd(t_command *cmd);

// tokenizer.c
t_exit_status					tokenize_with_qmap(const char *str,
									t_token_list *tokens);
int								process_quotes(const char *str,
									t_lexer_context *ctx);
int								process_spaces_outside_quotes(const char *str,
									t_token_list *tokens, t_lexer_context *ctx);
void							init_lexer_context(t_lexer_context *context);
t_token							*make_word_token(t_buf *buf);

// tokenizer_words.c
int								process_word(t_token_list *tokens,
									t_lexer_context *ctx);

// tokenizer_operators.c
int								check_operators(const char *str,
									t_token_list *tokens,
									t_lexer_context *context);
int								process_operator_token(t_token_type type,
									const char *literal, t_token_list *tokens,
									t_lexer_context *context);
int								should_handle_operator(const char *str,
									const t_lexer_context *ctx);
int								flush_word_buf_as_token(t_token_list *tokens,
									t_lexer_context *ctx);
int								dispatch_operator_token(const char *str,
									t_token_list *tokens, t_lexer_context *ctx);

// tokenizer_chars.c
int								append_char(char c, t_buf *buf,
									t_qmark quote_mark);
int								boost_buf(t_buf *buf, size_t needed_length);

// tokenizer_utils.c
bool							is_empty(const char *str);
bool							is_space(unsigned char c);
bool							is_operator(unsigned char c);
void							append_token(t_token_list *list,
									t_token *token);
void							reset_buf(t_buf *buf);
void							free_buf(t_buf *buf);
void							init_buffer(t_buf *buf);

// heredoc
t_exit_status					process_heredoc(t_pipeline *pl,
									t_env_var_list *env_vars,
									t_exit_status last_status);
int								expand_heredoc(char **line,
									t_env_var_list *env_vars,
									t_exit_status last_status);
int								hd_exp_dollar(char **dst, const char *src,
									size_t *i, t_exp_ctx *ctx);
char							*exp_varname(const char *src, size_t start,
									size_t len);
void							redir_replace_with_infile(t_redir *r,
									char *filename);
int								write_line_in_fd(int fd, char *line);
int								append_charter(char **line, char c);
int								append_string(char **line, const char *str);
t_exit_status					hd_abort(int fd, char *filename,
									t_exit_status st);

// execution.c
int								execute_pipeline(t_shell *shell);

// execution_forking.c
int								exec_pipeline_forking(t_shell *shell,
									const t_pipeline *pl);

// execution_forking_helpers.c
void							ctx_init(t_fork_ctx *ctx, pid_t *pids);
int								open_pipe_if_needed(t_fork_ctx *c,
									const t_pipeline *pl);
void							free_on_error(pid_t *pids, t_fork_ctx *c);
int								open_redir_file(const t_redir *redir);
void							apply_redirs_or_die(const t_command *cmd);

// exec_command_filter.c
int								is_builtin(const char *cmd);
int								is_parent_builtin(const char *cmd);
int								exec_builtin_in_parent(t_shell *shell,
									t_command *cmd);
int								run_any_builtin_in_child(t_shell *shell,
									t_command *cmd);
int								execute_built_in_commands(t_shell *shell,
									t_command *cmd);

// exec_builtin_commands.c
int								execute_echo(t_command *cmds);
int								execute_cd(t_command *cmds);
int								execute_pwd(void);
int								execute_exit(t_shell *shell, t_command *cmd);

// exec_builtin_cmds_utils.c
char							*fetch_current_working_directory(void);
bool							is_line_return(char **cmd, int *i);
bool							is_numeric_string(const char *str);
int								parse_exit_code(const char *str,
									int *exit_code);

// exec_builtin_env_commands.c
int								execute_export(t_shell *shell);
int								execute_unset(t_shell *shell);
int								execute_env(t_shell *shell);

// exec_builtin_env_cmds_utils.c
void							print_export(t_shell *shell);
void							sort_envp_alpha(char **envp);
void							str_swap(char **s1, char **s2);
int								key_check(char **key, char *argv);
int								check_data(char ***var_data, char *argv,
									char **key, char **value);
int								key_value_check_init(char **key,
									char ***var_data, char **value);

// exec_external_commands.c
int								execute_external_commands(t_shell *shell,
									t_command *cmd);

// exec_external_cmds_utils.c
bool							is_input_exec_ok(char *cmd, char **updt_path,
									bool *path_alloc);
bool							is_binary_found(char **updt_path,
									t_shell *shell, char *cmd,
									bool *path_alloc);
char							*fetch_and_check_bin_path(t_shell *shell,
									char *cmd);
char							*build_path_to_check(char *dir, char *cmd);
int								execve_fail(bool path_alloc, char **path,
									char ***envp, const char *name);

// exec_utils_fd.c
int								open_fd(const char *path, bool append,
									bool truncate);
void							open_and_close_fd(t_command *cmd);
int								pipe_setup(int *pipefds, size_t *i,
									size_t pl_count, pid_t *pids);
void							close_if_valid(int fd);
void							close_all_if_valid(int *fd, int *pipe_fd,
									bool exclude_read_pipe);

// exec_close_and_free.c
void							free_envp(t_env_var_list *list);
void							free_strings_array(char **array);

#endif
