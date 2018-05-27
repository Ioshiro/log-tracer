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

void start_time (int sig) {
  //inizializzo il tempo iniziale
  tstart = time(NULL);
  printf("\n **SIGUSR1 called and done** \n");
  return;
}

void end_time () {
  //inizializzo il tempo finale e calcolo il tempo di esecuzione
  int fd[2];
  char buf[MAX_TIME], buf2[MAX_TIME], buftot[MAX_TIME], c_path[MAX], c_value[MAX];
  char* command_str = "Command ";
  char* start_str = "Start time ";
  char* end_str = "End time ";
  char* time_spent_str = "Time spent processing ";
  char* value_str = "Value ";
//  int lung = strlen(c_path);
//  char format[4] = {c_path[lung-3], c_path[lung-2], c_path[lung-1], 0};
  char c_command [MAX];
  int command = open ("/tmp/command.tmp", O_RDONLY);
  int j = read (command, c_command, MAX);
  c_command[j] = 00;
  int d_format = open("/tmp/format.tmp", O_RDONLY);
  char format[MAX];
  read (d_format, format, 3);
  close (d_format);
  format[3]=00;
  tend = time(NULL);
  double ttot = difftime(tend,tstart);
  int ttotint = (int) ttot;
  printf("ttotint e' = %i\n", ttotint);
  strftime(buf,20,"%Y-%m-%d %H:%M:%S",localtime(&tstart));
  strftime(buf2,20,"%Y-%m-%d %H:%M:%S",localtime(&tend));
  snprintf(buftot,20, "%i", ttotint);
  //mi viene passato il path del file di output
  int path = open("/tmp/path.tmp", O_RDONLY);
  int i = read(path,c_path,MAX);
  printf("Primo i = %i \n", i);
  c_path[i] = 00;
  printf("Ho letto cosi': %s \n",format);
  printf("il path e' :%s\n",c_path);
  //remove(c_path);
  close(path);
  int value = open("/tmp/rvalue.tmp", O_RDONLY);
  read(value, c_value, 2);
  c_value[1] = 00;
  close(value);
  printf("Ho letto value: %s \n",c_value);
    //stampo i dati nel file di output
  //remove(c_path);
  fd[0] = open(c_path , O_WRONLY | O_CREAT | O_APPEND, 0777);
  if(strcmp(format,"txt") == 0) {
    write(fd[0], command_str, strlen(command_str));
    write(fd[0], c_command, j);
    write(fd[0], "\n", 1);
    write(fd[0], start_str, strlen(start_str));
    write(fd[0], buf, strlen(buf));
    write(fd[0], "\n", 1);
    write(fd[0], end_str, strlen(end_str));
    write(fd[0], buf2, strlen(buf2));
    write(fd[0], "\n", 1);
    write(fd[0], time_spent_str, strlen(time_spent_str));
    write(fd[0], buftot, strlen(buftot));
    write(fd[0], "\n", 1);
    write(fd[0], value_str, strlen(value_str));
    write(fd[0], c_value, strlen(c_value));
    close(fd[0]);
  }else if(strcmp(format,"csv") == 0){
    write(fd[0], command_str, strlen(command_str));
    write(fd[0], ",",1);
    write(fd[0], start_str, strlen(start_str));
    write(fd[0], ",", 1);
    write(fd[0], end_str, strlen(end_str));
    write(fd[0], ",", 1);
    write(fd[0], time_spent_str, strlen(time_spent_str));
    write(fd[0], ",", 1);
    write(fd[0], value_str, strlen(value_str));
    write(fd[0], "\n", 1);
    write(fd[0], c_command, j);
    write(fd[0], ",", 1);
    write(fd[0], buf, strlen(buf));
    write(fd[0], ",", 1);
    write(fd[0], buf2, strlen(buf2));
    write(fd[0], ",", 1);
    write(fd[0], buftot, strlen(buftot));
    write(fd[0], ",", 1);
    write(fd[0], c_value, strlen(c_value));
    write(fd[0], "\n", 1);
    close(fd[0]);
  }else if(strcmp(format,"xml") == 0){
    //write(fd[0], "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", MAX);
    write(fd[0], "<Command>", 9);
    write(fd[0], c_command, j);
    write(fd[0], "</Command>\n", 11);
    write(fd[0], "<Start time>", 12);
    write(fd[0], buf, strlen(buf));
    write(fd[0], "</Start time>\n", 14);
    write(fd[0], "<End time>", 10);
    write(fd[0], buf2, strlen(buf2));
    write(fd[0], "</End time>\n", 12);
    write(fd[0], "<Time spent processing>", 23);
    write(fd[0], buftot, strlen(buftot));
    write(fd[0], "</Time spent processing>\n", 25);
    write(fd[0], "<Value>", 7);
    write(fd[0], c_value, strlen(c_value));
    write(fd[0], "</Value>", 8);
    close(fd[0]);
  }else {
    printf("Formato non supportato\n");
  }
  printf("\n **SIGUSR2 called and done** \n");
  return;
}

void check () {
	remove(/tmp/logpid.tmp);
	exit (0);
}

int main () {
        //invio il pid al main e aspetto che mi risponda
	pid_t pid;
	pid = getpid();
	printf("my pid is %i\n", pid);
	char c_pid [MAX];
	sprintf(c_pid, "%i", pid);

	remove ("/tmp/logpid.tmp");
	int pid_log = open("/tmp/logpid.tmp", O_WRONLY | O_CREAT, 0777);
	write(pid_log, c_pid, strlen(c_pid));
	close(pid_log);

	//calcolo il tempo iniziale quando il main mi dice di iniziare
	signal(SIGUSR1, start_time);
	//calcolo il tempo finale quando il main mi "sveglia"
	signal(SIGUSR2, end_time);
	signal(SIGINT,check);
	//dormo aspettando il main
	while (1)
		sleep(1);
	exit(EXIT_SUCCESS);
}
