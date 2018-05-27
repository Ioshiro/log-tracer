#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#define MAX 5
#define READ 0
#define WRITE 1

char* pipe_toks[3];

char* path_tokenize(char* old_path){
	char* new_path;
	char ctrl[10]="--logfile";
	char c[2]= "=";
	new_path = strtok(old_path, c);
	if(!strcmp(new_path,ctrl)){
		new_path = strtok(NULL, c);
		return new_path;
	}
	else{
		perror("Sintassi di percorso errata. Usare --logfile=path \n");
		exit(EXIT_FAILURE);
	}
}

char* format_tokenize(char* old_format){
	char* new_format;
	char ctrl[9]="--format";
	char c[2] = "=";
	new_format = strtok(old_format, c);
	if(strcmp(new_format,ctrl)==0){
		new_format = strtok(NULL, c);
		return new_format;
	}
	else{
		perror("Sintassi di formato errata. Usare --format=txt/csv \n");
		exit(EXIT_FAILURE);
	}
}

void path_write(char* path){
	int fd[2];
	remove("/tmp/path.tmp");
	fd[0] = open("/tmp/path.tmp", O_WRONLY | O_CREAT, 0777);
	printf("pats is=%s\n",path);
	write(fd[0], path, strlen(path));
  close(fd[0]);
}

void format_write(char* format){
	int fd[2];
	fd[0] = open("/tmp/format.tmp", O_WRONLY | O_CREAT, 0777);
	write(fd[0], format, strlen(format));
  close(fd[0]);
}

void r_value_write(char* argv){
	int r = WEXITSTATUS(system(argv));
	int fd[2];
	fd[0]= open("/tmp/rvalue.tmp", O_WRONLY | O_CREAT, 0777);
	char r_value[MAX];
	sprintf(r_value, "%i \n", r);
	write(fd[0], r_value, strlen(r_value));
	close(fd[0]);
}

void command_write(char* command){
	remove("/tmp/command.tmp");
	int fd[2];
	fd[0] = open("/tmp/command.tmp", O_WRONLY | O_CREAT, 0777);
	write(fd[0], command, strlen(command));
	close(fd[0]);
}

int pipe_check (char* command){
	char c = '|';
	for(int i=0; i < strlen(command); i++){
		if(command[i] == c){
			return 1; // pipe-run
		}
	}
	return 0; // solo-run
}

void pipe_tokenize(char* pipeline){
	char c[2] = "|";
	printf("pipeline : %s\n", pipeline);
	pipe_toks[0] = strtok(pipeline,c);
	pipe_toks[1] = strtok(NULL,c);
	printf("%s\n", pipe_toks[0]);
	printf("%s\n", pipe_toks[1]); }

int filecheck() {
	char filename[15] = "/tmp/logpid.tmp";
	int find = 0;
	if (!access(filename,F_OK)) {
		find = 1;
	}
	return find;
}



int main (int argc, char** argv) {
	if (argc!=4) {
		perror("Sinstassi errata. La sintassi corretta e': ./run --pathfile=path --format=frmt comandi");
		exit(EXIT_FAILURE);
	}
	int stronzo;
	int g = filecheck();
	printf("Il risultato del check e' = %i\n", g);
	if (g == 0) {
		pid_t pid1 = fork();
		if (pid1 == 0) {
			printf("sono entrato nel figlio\n");
			static char* paramlist[] = {"log", NULL};
			stronzo = execv("/home/dimui/Programmazione/log-tracer/final-form-2/log", paramlist);
			fflush(NULL);
			exit(127);
		}else {
		int pid_log = open("/tmp/logpid.tmp", O_RDONLY);
		char c_log[MAX];
		char* path_tok;
		char* format_tok;

		read(pid_log, c_log, MAX);
		close(pid_log);
		int pid = atoi(c_log);
		printf("%i\n",pid);

		path_tok = path_tokenize(argv[1]);
		path_write(path_tok);
		format_tok = format_tokenize(argv[2]);
		format_write(format_tok);
		printf("Tok1: %s \n",path_tok);
		printf("Tok2: %s \n",format_tok);
		remove(path_tok);
		printf("path_tok = %s\n", path_tok);

		int cpipe = pipe_check(argv[3]);
			if(cpipe == 0){
				printf("Solo-run! \n");
				kill(pid, SIGUSR1);
				r_value_write(argv[3]);
				command_write(argv[3]);
				kill(pid, SIGUSR2);
				kill(pid,SIGINT);
			}
			else{
				printf("Wombo-combo! \n");
				pipe_tokenize(argv[3]);
				int pipex[2];
				pipe(pipex);
				if (fork() == 0) {
					close(pipex[READ]);
					dup2(pipex[WRITE],1);
					kill(pid, SIGUSR1);
					r_value_write(pipe_toks[0]);
					command_write(pipe_toks[0]);
					close(pipex[WRITE]);
					kill(pid, SIGUSR2);
					kill(pid,SIGINT);
				}else{
					wait(NULL);
					close(pipex[WRITE]);
					dup2(pipex[READ],0);
					kill(pid, SIGUSR1);
					r_value_write(pipe_toks[1]);
					command_write(pipe_toks[1]);
					close(pipex[READ]);
					kill(pid, SIGUSR2);
					kill(pid,SIGINT);
				}
			}
		}
	}else {
	int pid_log = open("/tmp/logpid.tmp", O_RDONLY);
	char c_log[MAX];
	char* path_tok;
	char* format_tok;

	read(pid_log, c_log, MAX);
	close(pid_log);
	int pid = atoi(c_log);
	printf("%i\n",pid);


	path_tok = path_tokenize(argv[1]);
	path_write(path_tok);
	format_tok = format_tokenize(argv[2]);
	format_write(format_tok);
	printf("Tok1: %s \n",path_tok);
	printf("Tok2: %s \n",format_tok);
	remove(path_tok);
	printf("path_tok = %s\n", path_tok);

	int cpipe = pipe_check(argv[3]);
		if(cpipe == 0){
			printf("Solo-run! \n");
			kill(pid, SIGUSR1);
			r_value_write(argv[3]);
			command_write(argv[3]);
			kill(pid, SIGUSR2);
		}
		else{
			printf("Wombo-combo! \n");
			pipe_tokenize(argv[3]);
			int pipex[2];
			pipe(pipex);
			if (fork() == 0) {
				close(pipex[READ]);
				dup2(pipex[WRITE],1);
				kill(pid, SIGUSR1);
				r_value_write(pipe_toks[0]);
				command_write(pipe_toks[0]);
				close(pipex[WRITE]);
				kill(pid, SIGUSR2);
			}else{
				wait(NULL);
				close(pipex[WRITE]);
				dup2(pipex[READ],0);
				kill(pid, SIGUSR1);
				r_value_write(pipe_toks[1]);
				command_write(pipe_toks[1]);
				close(pipex[READ]);
				kill(pid, SIGUSR2);
			}
		}
	}
	exit(EXIT_SUCCESS);
}
