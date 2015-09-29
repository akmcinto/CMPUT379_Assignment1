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
    
    if ((pid = fork()) < 0) {
      printf("fork() error!");

    } else if (pid == 0) {
      time(&currtime);
      fprintf(LOGFILE, "[%s]", ctime(&currtime));
      fclose(LOGFILE)
  
      //sprintf(cmdline, "kill $(pgrep %s)", procname);
      sprintf(cmdline, "pgrep %s", procname);
      printf("%s\n", cmdline);
      pp = popen(cmdline, "r");

      while (fscanf(pp, "%d", &procid) != EOF) {
	kill(procid, SIGKILL);
      }

      kill(pid, SIGKILL); // kill child
    } else {
      
      printf("Parent process\n");
      
    }

  }
  
  return 0;
}
