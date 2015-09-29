#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{

  // Open file
  FILE *f = fopen(argv[1], "r");
  // Array for holding each line of the file read in
  char procname[255]; // for saving read from file
  char cmdline[261]; // for creating pgrep command
  int numSeconds;
  pid_t pid;
  FILE *pp;
  
  int procid[5];
  
  fscanf(f, "%i", &numSeconds);

  // Test: print seconds
  // printf("%d\n", numSeconds);

  while (fscanf(f, "%s", procname) != EOF) {
    
    /*if ((pid = fork()) < 0) {
      printf("fork() error!");

    } else if (pid == 0) {
      /* pp = popen("ps", "r");
	 fscanf(pp, "%s %s %s %s", pspid, pstty, pstime, pscmd);
	 printf("%s\n",pspid);
	 printf("%s\n", line);
	 while (fscanf(pp, "%s %s %s %s", pspid, pstty, pstime, pscmd) != EOF) {
	 if (pscmd == line) {
	 kill((int) pspid, SIGKILL);
	 printf("%s program killed!", line);
	 break;
	 }*/
    sprintf(cmdline, "pgrep %s", procname);
    printf("%s\n", cmdline);
    pp = popen(cmdline, "r");
    fscanf(pp, "%d", procid);
    printf("%d\n", *procid);
      
      /*} else {
      
      printf("Parent process\n");
      }*/
  }

  return 0;
}
