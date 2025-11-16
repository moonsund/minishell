#include <stdio.h>
#include <unistd.h>

int main() {
	char	*ppath = "/bin/ls";							// Chemin absolu vers le programme à exécuter (ex: /bin/ls)
	char	*argv[] = {"/bin/ls", "-l", NULL};			// Tableau de strings (arguments du programme)
	char	*envp[] = {NULL};							// (Facultatif) - Tableau de strings (variables d’environnement)

	ft_printf("Avant execve\n");
	execve(ppath, argv, envp);
	ft_printf("This will never be printed\n");				// Ne sera pas affiché car ls a remplacé le binaire précédent
	return 0;
}
