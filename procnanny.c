#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "memwatch.h"

void killprevprocnanny( void );

int main(int argc, char *argv[])
{
  mwInit();

  // Log file
  char *logloc = getenv("PROCNANNYLOGS");
  FILE *LOGFILE = fopen(logloc, "w");
  // Open input file
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
  pid_t childpids[128];
  int childcount = 0;
  int killcount = 0;
  // Check if there were actually any process with that name
  int entered; 

  // kill any other procnannies
  killprevprocnanny();
  
  fscanf(f, "%i", &numsecs);

  while (fscanf(f, "%s", procname) != EOF) {
    // Find PIDs for the program
    sprintf(cmdline, "pgrep %s", procname);
    pp = popen(cmdline, "r");

    entered = 0;
    while (fscanf(pp, "%d", &procid) != EOF) {
      entered = 1;

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
	  fflush(LOGFILE);
	  exit(7); 
	}
	else {
	  exit(8);
	}	
      } 
      else { // parent process
	childpids[childcount] = pid;
	childcount++;
      }      
    }
    // end of pid while loop
    if (entered == 0) {
      time(&currtime);
      fprintf(LOGFILE, "[%.*s] Info: No '%s' process found.\n", (int) strlen(ctime(&currtime))-1, ctime(&currtime), procname);
      fflush(LOGFILE);
    }
  }
  // end of proc name while loop
  int i;
  for (i = 0; i < childcount; i++) {
    waitpid(childpids[i], &status, 0);
    if (WEXITSTATUS(status) == 7) {
      killcount++;
    }
  }

  time(&currtime);
  fprintf(LOGFILE, "[%.*s] Info: Exiting. %d process(es) killed.\n", (int) strlen(ctime(&currtime))-1, ctime(&currtime), killcount);

  //kill(pid, SIGTERM);
  
  fflush(LOGFILE);
  fclose(LOGFILE); 
  mwTerm();
  return 0;
}

// kill any previous instances of procnanny
void killprevprocnanny() {
  char pgrepcmd[20]; // for creating pgrep command (255 plus extra for command)
  FILE *pni;
  pid_t procnanid;
  pid_t mypid = getpid();

  // Find PIDs for the program
  sprintf(pgrepcmd, "pgrep %s", "procnanny");
  pni = popen(pgrepcmd, "r");
  
  while (fscanf(pni, "%d", &procnanid) != EOF) {
    if (procnanid != mypid) {
      kill(procnanid, SIGKILL);
    }
  }
  return;
}
