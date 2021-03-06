#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include "log.h"
#define MAX_DIM 5
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
	char r_value[MAX_DIM];
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
	char c[2] = "|"; //using standart delim char for pipe
	pipe_toks[0] = strtok(pipeline,c);
	pipe_toks[1] = strtok(NULL,c);
	return;
}

int main (int argc, char** argv) {
	//Syntax control
	if (argc!=4){
		perror("Sinstassi errata. La sintassi corretta e': ./run --logfile=path --format=frmt comandi");
		exit(EXIT_FAILURE);
	}

	//Creating the logger process
	if(fork()== 0)
	go_to_log();
	else{

	char c_log[MAX_DIM];
	char* path_tok;
	char* format_tok;

	//Reading pid from log.c /tmp file
	int pid_log = open("/tmp/logpid.tmp", O_RDONLY);
	read(pid_log, c_log, MAX_DIM);
	close(pid_log);
	int pid = atoi(c_log);

	//Reading, tokenizing and writing in /tmp path and format arguments
	path_tok = path_tokenize(argv[1]);
	path_write(path_tok);
	remove(path_tok);
	format_tok = format_tokenize(argv[2]);
	format_write(format_tok);

	//Control for pipe and eventual execution
	int cpipe = pipe_check(argv[3]);
		if(cpipe == 0){
			//One command execution
			//kill(pid, SIGUSR1);	//Calling start time
			start_time();
			r_value_write(argv[3]);
			command_write(argv[3]);
			end_time();
			//kill(pid, SIGUSR2);	//Calling final time and write log
		}
		else{
			//Pipe command execution
			pipe_tokenize(argv[3]);
			int pipex[2];
			pipe(pipex);
			if (fork() == 0) {
				//Pipe-write side
				close(pipex[READ]);
				dup2(pipex[WRITE],1);
				//kill(pid, SIGUSR1); //Calling start time
				start_time();
				r_value_write(pipe_toks[0]);
				command_write(pipe_toks[0]);
				end_time();
				close(pipex[WRITE]);
				//kill(pid, SIGUSR2); //Calling final time and write log
			}else{
				//Pipe-read side
				wait(NULL);
				close(pipex[WRITE]);
				dup2(pipex[READ],0);
				//kill(pid, SIGUSR1); //Calling start time
				start_time();
				r_value_write(pipe_toks[1]);
				command_write(pipe_toks[1]);
				end_time();
				close(pipex[READ]);
				//kill(pid, SIGUSR2); //Calling final time and write log
			}
		}
	}
	exit(EXIT_SUCCESS);
}
