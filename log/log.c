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
  char buf[MAX_TIME], buf2[MAX_TIME], buftot[MAX_TIME], c_value[2];
  char* start_str = "Start time = ";
  char* end_str = "End time = ";
  char* time_spent_str = "Time spent processing = ";
  char* value_str = "Value = ";
  tend = time(NULL);
  double ttot = difftime(tend,tstart);
  strftime(buf,20,"%Y-%m-%d %H:%M:%S",localtime(&tstart));
  strftime(buf2,20,"%Y-%m-%d %H:%M:%S",localtime(&tend));
  snprintf(buftot,20, "%f", ttot);
  //mi viene passato il path del file di output
  int path = open("/tmp/path.tmp", O_RDONLY);
  char c_pathtmp[MAX];
  //char c_empty[MAX]="";
  int i = read(path,c_pathtmp,MAX);
  printf("Ho letto cosi': %s \n",c_pathtmp);
  //strcpy(c_path,c_empty);
  close(path);
  printf("Primo i = %i \n", i);
  path = open("/tmp/path.tmp", O_RDONLY);
  char c_path [i];
  read(path,c_path,i);
  printf("Ho letto cosi': %s \n",c_path);
  printf("Secondo i = %i \n", i);
  close(path);
  int value = open("/tmp/rvalue.tmp", O_RDONLY);
  read(value, c_value, 2);
  close(value);
  //stampo i dati nel file di output
  fd[0] = open(c_path , O_WRONLY | O_APPEND | O_CREAT, 0777);
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
  printf("\n **SIGUSR2 called and done** \n");
  return;
}

int main () {
        //invio il pid al main e aspetto che mi risponda
	pid_t pid;
	pid = getpid();
	int pid_log = open("/tmp/log.tmp", O_WRONLY | O_CREAT, 0777);
	char c_pid [MAX];
	sprintf(c_pid, "%i", pid);
	write(pid_log, c_pid, strlen(c_pid));
	close(pid_log);
	//calcolo il tempo iniziale quando il main mi dice di iniziare
	signal(SIGUSR1, start_time);
	//calcolo il tempo finale quando il main mi "sveglia"
	signal(SIGUSR2, end_time);
	//dormo aspettando il main
	while (1)
		sleep(1);
	exit(EXIT_SUCCESS);
}
