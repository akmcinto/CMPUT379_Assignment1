#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

int main(int argc, char *argv[])
{
  FILE *LOGFILE = fopen("./logfile.txt", "w");
  // Open file
  FILE *f = fopen(argv[1], "r");
  // Array for holding each line of the file read in
  char procname[255]; // for saving read from file
  char cmdline[269]; // for creating pgrep command (255 plus extra for command)
  int numSeconds;
  pid_t pid;
  FILE *pp;
  pid_t procid;
  time_t currtime;
  
  fscanf(f, "%i", &numSeconds);

  while (fscanf(f, "%s", procname) != EOF) {
    // Find PIDs for the program
    sprintf(cmdline, "pgrep %s", procname);
    printf("%s\n", cmdline);
    pp = popen(cmdline, "r");

    while (fscanf(pp, "%d", &procid) != EOF) {

      if ((pid = fork()) < 0) {
	printf("fork() error!");
      } else if (pid == 0) {  // Child process

	// Initialize monitoring in log file
	time(&currtime);
	fprintf(LOGFILE, "[%s] Info: Initializing monitoring process %s (PID %d)\n", ctime(&currtime), procname, procid);
	fclose(LOGFILE);

	// Kill monitored process
	kill(procid, SIGKILL);
	// Kill child process
	kill(pid, SIGKILL); 
	
      } else {
      
	printf("Parent process\n");
      
      }
      
    }
    fclose(LOGFILE); 
  }
  
  return 0;
}
