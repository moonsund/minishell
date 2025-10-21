#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>    // EXIT_FAILURE, EXIT_SUCCESS
#include <limits.h>     // INT_MAX
#include <sys/time.h>  // time
# include <unistd.h>   // close, pipe, fork, read, write
# include <stdio.h>    // perror
# include <errno.h>    // errno
# include <sys/wait.h> // waitpid
# include <stdbool.h>




typedef struct t_shell
{
    int exit_status;







}   t_shell;







#endif