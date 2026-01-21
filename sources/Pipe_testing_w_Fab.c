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

int	main(void)
{
	int		fd[2] = {STDIN_FILENO, STDOUT_FILENO};		// fd[0] = in --- fd[1] = out
	int		pipe_fd[2] = {-1, -1};						// fd[0] = read - fd[1] = write
	int		backup_stdout = dup(STDOUT_FILENO);			// Penser à le fermer
	char *ar1[] = {"ls", NULL};
	char *ar2[] = {"grep", "main.c", NULL};
	char *envp[] = {NULL};
	char *s = calloc(1, 100);
	pid_t	pid_ls = 0;
	pid_t	pid_grep = 0;

	pipe(pipe_fd);
	pid_ls = fork();
	if (pid_ls == 0)
	{													// Entrée/Input de ls inchangé = STDIN = 0 / Son output est l'entrée du pipe
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);				// pipe_fd[1] devient STDOUT_FILENO
		close(pipe_fd[1]);
		pipe_fd[1] = -1;
		execve("/bin/ls", ar1, envp);
	}
	close(pipe_fd[1]);									// fermer l'entrée coté parent pour dire à grep que on a trouvé EOF, sinon il tourne à chercher

// Le processus parent n'a pas besoin d'interagir avec le pipe, mais doit fermer l'entree + sortie pour etre sur de ne rien bloquer

	pid_grep = fork();
	if (pid_grep == 0)
	{
		close(pipe_fd[1]);
		dup2(pipe_fd[0], STDIN_FILENO);
		close(pipe_fd[0]);
		pipe_fd[0] = -1;
		execve("/bin/grep", ar2, envp);
	}
	close(pipe_fd[0]);									// Pas indispensable mais bonne pratique
	waitpid(pid_ls, NULL, 0);
	waitpid(pid_grep, NULL, 0);

	return 0;
}
