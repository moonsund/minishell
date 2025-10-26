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

typedef enum e_token_type
{
    WORD,

} t_token_type;



typedef struct s_token
{
    char *tokeh;
} t_token;


typedef struct s_shell
{
	int		exit_status;
    char *normalized_cmd_str;
	char	**command_array;
}	t_shell;


// normalize
char *normalize_str(const char *str);

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
