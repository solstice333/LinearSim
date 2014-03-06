#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DEBUG 1

int main(int argc, char **argv) {
   double left, right;
   int time, cells; 

   if (4 != scanf(" %lf %lf %d %d", &left, &right, &time, &cells)) {
      fprintf(stderr, "Bad input. Exiting");
      exit(1);
   }

#if DEBUG
   printf("left: %0.1lf\n", left);
   printf("right: %0.1lf\n", right);
   printf("time: %d\n", time);
   printf("cells: %d\n", cells);
#endif

   return 0;
}
