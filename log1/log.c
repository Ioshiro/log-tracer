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


time_t tstart,tend;

void StartTime (int sig) {
  //inizializzo il tempo iniziale
  tstart = time(NULL);

}

void EndTime () {
  //inizializzo il tempo finale e calcolo il tempo di esecuzione
  tend = time(NULL);
  double ttot = difftime(tend,tstart);
  char buf[20], buf2[20], buftot[20];
  strftime(buf,20,"%Y-%m-%d %H:%M:%S",localtime(&tstart));
  strftime(buf2,20,"%Y-%m-%d %H:%M:%S",localtime(&tend));
  snprintf(buftot,20, "%f", ttot);
  printf("buff: %s \n",buf);
  int fd[2];
  char* start_str = "Start time = ";
  char* end_str = "End time = ";
  char* time_spent_str = "Time spent processing: ";
  //mi viene passato il path del file di output
  int path = open ("/tmp/path", O_RDONLY);
  char c_path[MAX];
  read(path, c_path, MAX);
  int value = open("/tmp/rvalue", O_RDONLY);
  char c_value[2];
  char* value_str = "Value = ";
  read(value, c_value, 2);
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
  write(fd[0], c_value, strlen(c_value));// **QUESTO NON VA**
  close(fd[0]);
  exit(0);
}

int main () {
        //invio il pid al main e aspetto che mi risponda
	pid_t pid;
	pid = getpid();
	int pid_log = open("/tmp/log", O_WRONLY | O_CREAT, 0777);
	char c_pid [MAX];
	sprintf(c_pid, "%i", pid);
	write(pid_log, c_pid, strlen(c_pid));
	close(pid_log);
	//calcolo il tempo iniziale quando il main mi dice di iniziare
	signal(SIGUSR1, StartTime);
	kill(pid,SIGUSR1);
	//calcolo il tempo finale quando il main mi "sveglia"
	signal(SIGUSR2, EndTime);
	kill(pid,SIGUSR2);

	exit(EXIT_SUCCESS);
	//dormo aspettando il main
	while (1)
		sleep(1);
	return 0;
}
