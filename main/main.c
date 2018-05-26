#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#define MAX 5

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
	if(!strcmp(new_format,ctrl)){
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
	fd[0] = open("/tmp/path.tmp", O_WRONLY | O_CREAT, 0777);
	write(fd[0], path, strlen(path));
  close(fd[0]);
	return;
}

void format_write(char* format){
	int fd[2];
	fd[0] = open("/tmp/format.tmp", O_WRONLY | O_CREAT, 0777);
	write(fd[0], format, strlen(format));
  close(fd[0]);
	return;
}

void r_value_write(char* argv){
	int r = WEXITSTATUS(system(argv));
	int fd[2];
	fd[0]= open("/tmp/rvalue.tmp", O_WRONLY | O_CREAT, 0777);
	char r_value[MAX];
	sprintf(r_value, "%i \n", r);
	write(fd[0], r_value, strlen(r_value));
	close(fd[0]);
	return;
}

void command_write(char* command){
	int fd[2];
	fd[0] = open("/tmp/command.tmp", O_WRONLY | O_CREAT, 0777);
	write(fd[0], command, strlen(command));
	close(fd[0]);
	return;
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
	pipe_toks[0] = strtok(pipeline,c);
	pipe_toks[1] = strtok(NULL,c);
	return;
}


int main (int argc, char** argv) {
	if (argc!=4){
		perror("Sinstassi errata. La sintassi corretta e': ./run --pathfile=path --format=frmt comandi");
		exit(EXIT_FAILURE);
	}

	int pid_log = open("/tmp/log.tmp", O_RDONLY);
	char c_log[MAX];
	char* path_tok;
	char* format_tok;

	read(pid_log, c_log, MAX);
	close(pid_log);
	int pid = atoi(c_log);
	printf("%i\n",pid);

	kill(pid, SIGUSR1);

	path_tok = path_tokenize(argv[1]);
	path_write(path_tok);
	format_tok = format_tokenize(argv[2]);
	format_write(format_tok);
	printf("Tok1: %s \n",path_tok);
	printf("Tok2: %s \n",format_tok);

	int cpipe = pipe_check(argv[3]);
		if(cpipe == 0){
			printf("Solo-run! \n");
			r_value_write(argv[3]);
		}
		else{
			printf("Wombo-combo! \n");
			pipe_tokenize(argv[3]);
			r_value_write(pipe_toks[0]);
			r_value_write(pipe_toks[1]);
		}
	command_write(argv[3]);

	kill(pid, SIGUSR2);
	exit(EXIT_SUCCESS);
}
