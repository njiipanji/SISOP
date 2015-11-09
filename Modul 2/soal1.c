#include <stdio.h>		// fprintf(), printf(), stderr(), getchar(), perror()
#include <unistd.h>		// chdir(), fork(), exec(), pid_t
#include <sys/wait.h>		// waitpid()
#include <stdlib.h>		// malloc(), realloc(), free(), exit(), execvp(), EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>		// strcmp(), strtok()
#include <signal.h>		// signalHandler

#define RL_BUFSIZE 1024		// deklarasi macro variabel dari ukuran buffer pada baris
#define TOK_BUFSIZE 64		// deklarasi macro variabel dari ukuran buffer pada ****
#define TOK_DELIM " \t\r\n\a"	// deklarasi macro variabel dari delimiter

void signalHandler(int);	// fungsi untuk menangkap sinyal yang diberikan user
void run_loop(void);		// fungsi basic loop dari shell
char *read_line(void);		// fungsi membaca baris
char **split_line(char*);	// fungsi untuk memparsing baris menjadi beberapa argument
int shell_launch(char**);	// fungsi untuk menjalankan perintah yang diinput
int shell_cd(char**);		// fungsi deklarasi untuk perintah tambahan "cd"
int shell_help(char**);		// fungsi deklarasi untuk perintah tambahan "help"
int shell_exit(char**);		// fungsi deklarasi untuk perintah tambahan "exit"
int shell_execute(char**);	// fungsi untuk menjalankan perintah baik itu perintah tambahan atau proses normal (/bin)

// array dari perintah tambahan yang dimasukkan ke dalam program shell ini
char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

// array dari fungsi pointer yang mengambil array dari string dan mereturn nilai integer
int (*builtin_func[]) (char**) = {
	&shell_cd,
	&shell_help,
	&shell_exit
};

int num_builtins() {
	return sizeof(builtin_str) / sizeof(char*);
}

int main (int argc, char **argv)
{
	if (signal(SIGTSTP, signalHandler)==SIG_ERR) {
		printf ("\n Proccess can't catch SIGTSTP\n");
	}
	if (signal(SIGINT, signalHandler)==SIG_ERR) {
		printf ("\n Proccess can't catch SIGINT\n");
	}

	// memuat config files (jika ada)

	// menjalankan perintah loop
	run_loop();

	// perform any shutdown/cleanup

	return EXIT_SUCCESS;
}

void signalHandler(int signum)
{
	if (signum==SIGTSTP) {
		printf (" Proccess received SIGTSTP\n");
	}
	else if (signum==SIGINT) {
		printf (" Proccess received SIGINT\n");
	}
}

void run_loop(void)
{
	char *line;
	char **args;
	int status;

	do {
		char *username=getenv("USER");
	
		printf ("%s: ", username);	// menampilkan username
		line = read_line();		// memanggil fungsi untuk membaca baris perintah yang diinput user
		args = split_line(line);	// membagi baris perintah menjadi program dan arguments
		status = shell_execute(args);	// mengeksekusi argumen yang sudah terparsing dan mereturn nilai eksekusinya
		printf ("%d\n", status);

		free(line);
		free(args);
	} while(status);
}

char *read_line(void)
{
	int bufsize=RL_BUFSIZE;
	int position=0;
	char *buffer=malloc(sizeof(char) *bufsize);
	int c;		// menggunakan tipe data integer, untuk mendeteksi EOF (yang merupakan integer)

	// cek alokasi error atau tidak
	if (!buffer) {
		fprintf(stderr, "shell: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		c=getchar();	// membaca karakter yang diinput user
	
		// jika user menginstruksikan EOF, ubah dengan null character
		if (c==EOF || c=='\n') {
			buffer[position]='\0';
			return buffer;
		}
		else {
			buffer[position]=c;
		}
		position++;

		// jika melebihi buffer, mengalokasi ulang
		if (position >= bufsize) {
			bufsize += RL_BUFSIZE;
			buffer=realloc(buffer, bufsize);
			if (!buffer) {
				fprintf (stderr, "shell: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

char **split_line(char *line)
{
	int bufsize=TOK_BUFSIZE, position=0;
	char **tokens=malloc(bufsize *sizeof(char*));
	char *token;

	if (!tokens) {
		fprintf(stderr, "Shell: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token=strtok(line, TOK_DELIM);	// return pointers ke dalam string yang diinputkan, serta menempatkan karakter '\0' di akhir tiap token
	while (token!=NULL) {
		tokens[position]=token;
		position++;

		// alokasi ulang jika melebihi ukuran buffer yang disediakan
		if (position >= bufsize) {
			bufsize += TOK_BUFSIZE;
			tokens=realloc(tokens, bufsize *sizeof(char*));
			
			if (!tokens) {
				fprintf(stderr, "shell: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		
		token=strtok(NULL, TOK_DELIM);
	}

	tokens[position]=NULL;
	return tokens;
}

int shell_launch(char **args)
{
	pid_t pid, wpid;
	int status;

	pid=fork();
	if (pid==0) {
		// child process
		// menjalankan perintah baris yang diinput user
		if (execvp(args[0], args)==-1) {
			perror("Shell error");
		}
		exit (EXIT_FAILURE);
	}
	else if (pid<0) {
		// error forking
		perror("Shell error");
	}
	else {
		// parrent process
		do {
			wpid=waitpid(pid, &status, WUNTRACED);
			// menunggu proses (perintah yang diinput) selesai

		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}
			
int shell_cd(char **args)
{
	if (args[1]==NULL) {
		fprintf(stderr, "shell: expected argument to \"cd\"\n");
	}
	else {
		if (chdir(args[1]) != 0) {
			perror("Shell Error: ");
		}
	}

	return 1;
}

int shell_help(char **args)
{
	int i;

	printf ("Selamat datang di Shell Buatan\n");
	printf ("Masukkan nama program dan argument, kemudian tekan enter\n");
	printf ("Berikut ini daftar perintah tambahan yang ada di program ini:\n");
	
	for (i=0; i<num_builtins(); i++) {
		printf("  %s\n", builtin_str[i]);
	}

	printf ("Gunakan perintah 'man' untuk mengetahui informasi dari program tersebut\n");
	return 1;
}

int shell_exit(char **args)
{
	// mereturn nilai 0, dengan maksud sebagai sinyal kepada fungsi loop untuk memberhentikan program
	return 0;
}

int shell_execute(char **args)
{
	int i;

	if (args[0]==NULL) {
		// memasukkan sebuah perintah kosong
		return 1;
	}
	
	// cek apakah perintah yang dimasukkan merupakan perintah tambahan, jika iya-> dijalankan, jika tidak-> panggil shell_launch
	for (i=0; i<num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i])==0) {
			return (*builtin_func[i])(args);
		}
	}

	return shell_launch(args);
}
