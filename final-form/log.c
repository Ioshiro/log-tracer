#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define MAX 100
#define MAX_TIME 20


time_t tstart,tend;
int fd[2];
char c_tstart[MAX_TIME], c_tend[MAX_TIME], c_ttot[MAX_TIME];


void format_write_txt(){
  char* command_str = "Command ";
  char* start_str = "Start time ";
  char* end_str = "End time ";
  char* time_spent_str = "Time spent processing ";
  char* value_str = "Value ";

  //Reading command and return value
  char c_command [MAX];
  int d_command = open("/tmp/command.tmp", O_RDONLY);
  int j = read(d_command, c_command, MAX);
  close(d_command);
  c_command[j] = 00;
  char c_value[MAX];
  int d_value = open("/tmp/rvalue.tmp", O_RDONLY);
  read(d_value, c_value, 2);
  c_value[1] = 00;
  close(d_value);

  //Writing actual log info
  write(fd[0], command_str, strlen(command_str));
  write(fd[0], c_command, j);
  write(fd[0], "\n", 1);
  write(fd[0], start_str, strlen(start_str));
  write(fd[0], c_tstart, strlen(c_tstart));
  write(fd[0], "\n", 1);
  write(fd[0], end_str, strlen(end_str));
  write(fd[0], c_tend, strlen(c_tend));
  write(fd[0], "\n", 1);
  write(fd[0], time_spent_str, strlen(time_spent_str));
  write(fd[0], c_ttot, strlen(c_ttot));
  write(fd[0], "\n", 1);
  write(fd[0], value_str, strlen(value_str));
  write(fd[0], c_value, strlen(c_value));
  write(fd[0], "\n", 1);
  close(fd[0]);
  return;
}

void format_write_csv(){
  char* command_str = "Command ";
  char* start_str = "Start time ";
  char* end_str = "End time ";
  char* time_spent_str = "Time spent processing ";
  char* value_str = "Value ";

  //Reading command and return value
  char c_command [MAX];
  int d_command = open("/tmp/command.tmp", O_RDONLY);
  int j = read(d_command, c_command, MAX);
  close(d_command);
  c_command[j] = 00;
  char c_value[MAX];
  int d_value = open("/tmp/rvalue.tmp", O_RDONLY);
  read(d_value, c_value, 2);
  c_value[1] = 00;
  close(d_value);

  //Writing actual log info
  write(fd[0], command_str, strlen(command_str));
  write(fd[0], ",", 1);
  write(fd[0], start_str, strlen(start_str));
  write(fd[0], ",", 1);
  write(fd[0], end_str, strlen(end_str));
  write(fd[0], ",", 1);
  write(fd[0], time_spent_str, strlen(time_spent_str));
  write(fd[0], ",", 1);
  write(fd[0], value_str, strlen(value_str));
  write(fd[0], "\n", 1);
  write(fd[0], c_command, strlen(c_command));
  write(fd[0], ",", 1);
  write(fd[0], c_tstart, strlen(c_tstart));
  write(fd[0], ",", 1);
  write(fd[0], c_tend, strlen(c_tend));
  write(fd[0], ",", 1);
  write(fd[0], c_ttot, strlen(c_ttot));
  write(fd[0], ",", 1);
  write(fd[0], c_value, strlen(c_value));
  write(fd[0], "\n", 1);
  close(fd[0]);
  return;
}

void format_write_xml(){
  //Reading command and return value
  char c_command [MAX];
  int d_command = open("/tmp/command.tmp", O_RDONLY);
  int j = read(d_command, c_command, MAX);
  close(d_command);
  c_command[j] = 00;
  char c_value[MAX];
  int d_value = open("/tmp/rvalue.tmp", O_RDONLY);
  read(d_value, c_value, 2);
  c_value[1] = 00;
  close(d_value);

  //Writing actual log info
  write(fd[0], "<Command>", 9);
  write(fd[0], c_command, strlen(c_command));
  write(fd[0], "</Command>\n", 11);
  write(fd[0], "<Start time>", 12);
  write(fd[0], c_tstart, strlen(c_tstart));
  write(fd[0], "</Start time>\n", 14);
  write(fd[0], "<End time>", 10);
  write(fd[0], c_tend, strlen(c_tend));
  write(fd[0], "</End time>\n", 12);
  write(fd[0], "<Time spent processing>", 23);
  write(fd[0], c_ttot, strlen(c_ttot));
  write(fd[0], "</Time spent processing>\n", 25);
  write(fd[0], "<Value>", 7);
  write(fd[0], c_value, strlen(c_value));
  write(fd[0], "</Value>", 8);
  write(fd[0], "\n", 1);
  close(fd[0]);

  return;
}

//SIGUSR1
void start_time (int sig) {
  //Initializing start time
  tstart = time(NULL);
  return;
}

//SIGUSR2
void end_time () {
  //Recovering path and format from main-made files in /tmp
  char c_path[MAX];
  int d_path = open("/tmp/path.tmp", O_RDONLY);
  int i = read(d_path, c_path, MAX);
  close(d_path);
  c_path[i] = 00;
  char c_format[MAX];
  int d_format = open("/tmp/format.tmp", O_RDONLY);
  int k = read(d_format, c_format, MAX);
  close (d_format);
  c_format[k] = 00;

  //Initializing final time and taking execution time
  tend = time(NULL);
  double ttot = difftime(tend, tstart);
  int ttotint = (int) ttot;

  //Converting time_t to strings
  strftime(c_tstart, 20, "%Y-%m-%d %H:%M:%S", localtime(&tstart));
  strftime(c_tend, 20, "%Y-%m-%d %H:%M:%S", localtime(&tend));
  snprintf(c_ttot, 20, "%i", ttotint);

  //Writing on log file in different formats
  fd[0] = open(c_path , O_WRONLY | O_CREAT | O_APPEND, 0777);
  if(strcmp(c_format, "txt") == 0) {
    format_write_txt();
  }
  else if(strcmp(c_format, "csv") == 0){
    format_write_csv();
  }
  else if(strcmp(c_format,"xml") == 0){
    format_write_xml();
  }
  else {
    printf("Formato non supportato\n");
  }
  return;
}

int main () {
  //Initializing pid and converting to string
	pid_t pid;
	pid = getpid();
	char c_pid [MAX];
	sprintf(c_pid, "%i", pid);
	remove ("/tmp/logpid.tmp");

  //Saving pid for main in /tmp
	int pid_log = open("/tmp/logpid.tmp", O_WRONLY | O_CREAT, 0777);
	write(pid_log, c_pid, strlen(c_pid));
	close(pid_log);

  //Defining custom SIGUSR signals
	signal(SIGUSR1, start_time); //SIGUSR1 will only take start time
	signal(SIGUSR2, end_time); //SIGUSR2 will take final time, exe time and will write everything on log file

	//Sleeping while wainting for main calls
	while (1)
		sleep(1);
	exit(EXIT_SUCCESS);
}
