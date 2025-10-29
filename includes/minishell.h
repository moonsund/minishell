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
    WORD,
    PIPE,
    REDIR_IN,
    HEREDOC,
    REDIR_OUT,
    APPEND,




} t_token_type;

typedef struct s_token
{
    char *text;
    t_token_type type;
    struct s_token  *next;
} t_token;

typedef struct s_token_list
{
    t_token *head;
    unsigned int count;
} t_token_list;

typedef struct s_shell
{
	int		exit_status;
    char *normalized_cmd_str;
	char	**command_array;
    t_token_list list;
}	t_shell;


// normalize
char *normalize_str(const char *str);

// lexer
bool tokenize(const char *str, t_token_list *list);
void clean(t_token_list *list);

// utils
bool is_empty(const char *str);
bool is_space(unsigned char c);

// signals
void setup_signals(void);


// exec functions
void	execute_commands(t_shell command/*, TBD */);
void	execute_pwd();
void	execute_cd(char *requested_path);

#endif
