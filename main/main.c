#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#define MAX 5


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
		perror("Sintassi di path errata. Usare --logfile=path \n");
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
		perror("Sintassi di format errata. Usare --format=txt/csv \n");
		exit(EXIT_FAILURE);
	}
}

void path_save(char* path){
	int fd[2];
	fd[0] = open("/tmp/path.tmp", O_WRONLY | O_CREAT, 0777);
	write(fd[0], path, strlen(path));
  close(fd[0]);
	return;
}

void format_save(char* format){
	int fd[2];
	fd[0] = open("/tmp/format.tmp", O_WRONLY | O_CREAT, 0777);
	write(fd[0], format, strlen(format));
  close(fd[0]);
	return;
}

int main (int argc, char** argv) {
	if (argc!=3){
		perror("Sinstassi errata. La sintassi corretta e': ./run --pathfile=path --format=frmt comandi");
		exit(EXIT_FAILURE);
	}
	int pid_log = open("/tmp/log", O_RDONLY);
	char c_log[MAX];
	char* path_tok;
	char* format_tok;
	read(pid_log, c_log, MAX);
//	printf("%s\n",c_log);
	close(pid_log);
//	int pid = atoi(c_log);
//	kill(pid, SIGUSR1);
//	kill(pid, SIGUSR2);
	path_tok = path_tokenize(argv[1]);
	format_tok = format_tokenize(argv[2]);
	path_save(path_tok);
	format_save(format_tok);
	printf("Tok1: %s \n",path_tok);
	printf("Tok2: %s \n",format_tok);
	return 0;
}
