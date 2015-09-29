#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "memwatch.h"

int main(int argc, char *argv[])
{
  mwInit();

  FILE *LOGFILE = fopen("./logfile.txt", "w");
  // Open file
  FILE *f = fopen(argv[1], "r");
  int status = 0;
  // Array for holding each line of the file read in
  char procname[255]; // for saving read from file
  char cmdline[269]; // for creating pgrep command (255 plus extra for command)
  int numsecs;
  pid_t pid;
  FILE *pp;
  pid_t procid;
  time_t currtime;
  pid_t childpids[255];
  int childcount = 0;
  
  fscanf(f, "%i", &numsecs);

  while (fscanf(f, "%s", procname) != EOF) {
    // Find PIDs for the program
    sprintf(cmdline, "pgrep %s", procname);
    pp = popen(cmdline, "r");

    while (fscanf(pp, "%d", &procid) != EOF) {
      
      // Initialize monitoring in log file
      time(&currtime);
      fprintf(LOGFILE, "[%.*s] Info: Initializing monitoring process %s (PID %d)\n", (int) strlen(ctime(&currtime))-1, ctime(&currtime), procname, procid);
	
      fflush(LOGFILE);

      if ((pid = fork()) < 0) {
	printf("fork() error!");
      } 

      else if (pid == 0) {  // Child process
	// Wait for amount of time
	sleep(numsecs);

	// Kill monitored process
	int killed = kill(procid, SIGKILL);
	// If the process is actually killed, print to log
	if (killed == 0) {
	  time(&currtime);
	  fprintf(LOGFILE, "[%.*s] Action: PID %d (%s) killed after exceeding %d seconds\n", (int) strlen(ctime(&currtime))-1, ctime(&currtime), procid, procname, numsecs);
	}
	fflush(LOGFILE);
	exit(7);       
	
      } 
      else {
	childpids[childcount] = pid;
	//printf("%d\n", childpids[childcount]);
	childcount++;
	continue; 
      }      
    }  
  }
  
  int i;
  for (i = 0; i < childcount; i++) {
    if (wait(&status) != childpids[i]) {
      ;
    }
  }

  fprintf(LOGFILE, "[%.*s] Info: Exiting. %d process(es) killed.\n", (int) strlen(ctime(&currtime))-1, ctime(&currtime), childcount);

  //kill(pid, SIGTERM);
  
  fflush(LOGFILE);
  fclose(LOGFILE); 
  mwTerm();
  return 0;
}
