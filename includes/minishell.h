#ifndef MINISHELL_H
# define MINISHELL_H

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

#include <sys/types.h>	// opendir
#include <dirent.h>		// opendir

#include <ft_printf.h>

typedef enum e_token_type
{
	TOK_WORD,
	TOK_PIPE,		// |
	TOK_REDIR_IN,	// <
	TOK_HEREDOC,	// >
	TOK_REDIR_OUT,	// <<
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
	bool malloc_error;
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

typedef struct s_temp_command				// To delete after code matching
{
	char					**full_command;	// Exec requirement
	struct s_temp_command	*next;
	// demander a Leo pour infiles/outfiles - Qu'est ce qu'il va me donner du parsing pour que je bosse avec une redirection par exemple
}	t_temp_command;

typedef struct s_env
{
	char			*variable_name;
	char			*variable_data;
	struct s_env	*next;
}	t_env;

typedef struct s_shell
{
	int				exit_status;
	char			*normalized_cmd_str;	// To delete TBC
	char			**command_array;		// To delete TBC
	t_temp_command	all_commands;			// To delete after code matching
	t_env			**env_variables;		// Exec requirement
	t_token_list	tokens;					// The one to consider ? TBC
	t_pipeline		*pipeline;				// Abstract Syntax Tree
}	t_shell;

// utils
int		str_comp(char *s1, char *s2);

// pre exec functions
void	check_command_type_and_execute(t_shell minishell/*, TBD */);
void	execute_built_in_commands(t_shell minishell/*, TBD */);
void	execute_external_commands(t_shell minishell/*, TBD */);

// exec built in functions
void	execute_echo(char **command_array);
void	execute_cd(char *current_working_directory, char **command_array);
void	execute_pwd(char *current_working_directory);
void	execute_export(t_shell minishell);
void	execute_unset(t_shell minishell);
void	execute_env(t_shell minishell);
void	execute_exit(t_shell minishell);

// env related functions
t_env	**build_environment(void);
t_env	*create_new_environment_variable(char *key, char *value);
t_env	*search_last_var(t_env *env_var);
void	add_env_var_to_list(t_env **head, t_env *new);
void	delete_env_var(t_env **head, char *var_to_delete, void (*del_string)(void *));

// exec external functions
char	**execute_ls(t_shell minishell, char *path);

// Free functions
void	del_string(char *param);
void	free_all_vars(t_env **head);
void	free_everything(t_shell minishell);
#endif
