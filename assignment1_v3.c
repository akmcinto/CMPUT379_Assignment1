#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

int main(int argc, char *argv[])
{
  FILE *LOGFILE = fopen("./logfile.txt", "w");
  // Open file
  FILE *f = fopen(argv[1], "r");
  int status;
  // Array for holding each line of the file read in
  char procname[255]; // for saving read from file
  char cmdline[269]; // for creating pgrep command (255 plus extra for command)
  int numsecs;
  pid_t pid;
  FILE *pp;
  pid_t procid;
  time_t currtime;
  
  fscanf(f, "%i", &numsecs);

  while (fscanf(f, "%s", procname) != EOF) {
    // Find PIDs for the program
    sprintf(cmdline, "pgrep %s", procname);
    pp = popen(cmdline, "r");

    while (fscanf(pp, "%d", &procid) != EOF) {

      if ((pid = fork()) < 0) {
	printf("fork() error!");
      } 

      else if (pid == 0) {  // Child process
	// Initialize monitoring in log file
	time(&currtime);
	fprintf(LOGFILE, "[%23s] Info: Initializing monitoring process %s (PID %d)\n", ctime(&currtime), procname, procid);
	
	fflush(LOGFILE);
	// Wait for amount of time
	sleep(numsecs);

	// Kill monitored process
	int killed = kill(procid, SIGKILL);
	// If the process is actually killed, print to log
	if (killed == 0) {
	  fprintf(LOGFILE, "[%s] Action: PID %d (%s) killed after exceeding %d seconds\n", ctime(&currtime), procid, procname, numsecs);
	}
	fflush(LOGFILE);
	exit(7);       
	
      } 

      else {
	//printf("Parent process\n"); 
      }

      while (wait(&status) != pid) {
	//printf("wait error");
	;
      }
      kill(pid, SIGTERM);
      
    }

    
  }
  fclose(LOGFILE); 
  return 0;
}
