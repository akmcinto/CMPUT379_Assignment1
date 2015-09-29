#include <stdio.h>

int main(int argc, char *argv[])
{

  FILE *f = fopen(argv[1], "r");
  int c;
  
  while ((c = fgetc(f)) != EOF) {
    if (c == "\n") {
      printf("\n");
    }
    else {
      printf("%c", c);
    }
  };

  return 0;
}
