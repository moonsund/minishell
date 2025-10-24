#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
	printf("Finished reading\n");
	execve("ls", NULL, NULL);
	printf("OK\n");					// Ne sera pas affiché car ls a remplacé le binaire précédent
    return 0;
}
