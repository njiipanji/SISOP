#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#define MAX_COMMAND 150
#define MAX_PARAMETER 10

void signalHandler (int);
void parsingCommand(char*, char**);
int executeCommand(char**);
int flag=0;

int main (void)
{
	char ampersand;
	char command[MAX_COMMAND+1];
	char *parameter[MAX_PARAMETER+1];
	
	if (signal(SIGTSTP, signalHandler) == SIG_ERR)
		printf("\n Proccess can't catch SIGTSTP\n");
	if (signal(SIGINT, signalHandler) == SIG_ERR)
		printf("\n Proccess can't catch SIGSTOP\n");
	
	while(1) {
		// cetak username
		char *username=getenv("USER");
		printf ("%s@myshell> ", username);

		// baca perintah dari input user, CTRL+D->exit program
		if (fgets(command, sizeof(command), stdin) == NULL) break;

		// menghapus karakter '\n', diubah menjadi karakter '\0'
		if (command[strlen(command)-1] == '\n') {
			command[strlen(command)-1] = '\0';
		}
		
		// cek ampersand
		ampersand=command[strlen(command)-2];
		if ((int)ampersand == (int)'&') {
			flag=1;
			printf ("testing\n");
			command[strlen(command)-2]='\0';
		}
		else {
			flag=-1;	
		}

		// parsing command
		parsingCommand(command, parameter);

		// builtin command : cd, exit
		if (strcmp(parameter[0], "exit") == 0) break;
		else if (strcmp(parameter[0], "cd") == 0) {
			chdir(parameter[1]);
		}
		// eksekusi perintah
		else if (executeCommand(parameter) == 0) break;
	}

	return 0;
}

void signalHandler (int sigNum)
{
	if (sigNum == SIGTSTP) printf (" myShell received SIGTSTP\n");
	else if (sigNum == SIGINT) printf (" myShell received SIGINT\n");
}

void parsingCommand(char *command, char **parameter)
{
	int i;
	for (i=0; i<MAX_PARAMETER; i++) {
		parameter[i]=strsep(&command, " ");
		if (parameter[i]==NULL) break;
	}
}

int executeCommand(char **parameter)
{
	pid_t pid = fork();

	if (pid==0) {
		// eksekusi perintah
		execvp(parameter[0], parameter);
	}
	else {
		if (flag==-1) {
			// jika tidak terdapat '&' di akhir perintah
			int childStats;
			waitpid(pid, &childStats, 0);
			return 1;	
		}
	}
}
