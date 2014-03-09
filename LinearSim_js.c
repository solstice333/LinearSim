#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Report.h"

#define DEBUG 1
#define MAX_ARGS 8
#define MAX_ARG_SIZE 20

/* Builds an argument list that can be passed to execv.
 *
 * numPairs is the number of letter:number pairs that will be created.
 * Following numPairs are all arguments are treated as groups of 2.
 * The first argument in each pair is a char, and any char but 'V'
 * must be followed by an integer. If the char is 'V', follow it with
 * a double.
 *
 * Returns a double char pointer which contains strings representing
 * the parameter pairs and terminated by a NULL pointer.
 */
char** BuildArgs(int numPairs, ...){
   char **argList = calloc(MAX_ARGS, sizeof(char *));
   char *str = (char*) (&numPairs + 1);
   int argNdx = 0;

   //argList[argNdx++] = "Cell";
   //numPairs++;

   for (; argNdx < numPairs; argNdx++) {
      argList[argNdx] = calloc(MAX_ARG_SIZE, sizeof(char));

      if (*str == 'V') {
         sprintf(argList[argNdx], "%c%lf\0", *str,
          *(double *) (str + sizeof(int)));
         str += sizeof(int) + sizeof(double);
      }
      else {
         sprintf(argList[argNdx], "%c%d\0", *str, *(str + sizeof(int)));
         str += sizeof(int) * 2;
      }
   }

   argList[argNdx] = NULL;

   return argList;
}

int main(int argc, char **argv) {
   double left, right;
   int time, cells, id, ndx, isChild, forkNdx, status;
   int fds[3][2], reportFd[2];
   char **temp;
   Report statRep;
   pid_t childPid;

   id = forkNdx = 0;

   scanf(" %lf %lf %d %d", &left, &right, &time, &cells);
   if (time < 0)
      time = 0;
   if (cells < 1)
      cells = 1;

#if DEBUG
   printf("left: %0.1lf\n", left);
   printf("right: %0.1lf\n", right);
   printf("time: %d\n", time);
   printf("cells: %d\n", cells);

   temp = BuildArgs(5, 'S', 8, 'F', 10, 'V', 7.2345, 'T', 123, 'V', 9.0);
   printf("BuildArgs: %s %s %s %s %s %s\n",
    temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
#endif

   pipe(reportFd);
   pipe(fds[0]);

   if (!fork()) {
      close(reportFd[0]);
      close(fds[0][0]);
      execv("Cell", BuildArgs(5, 'D', 0, 'S', time, 'V', left, 'O',
       reportFd[1], 'O', fds[0][1]));
   }

   ++id;
   
   //more child logic
   
   close(reportFd[1]);
   //close(fds[0][0]);//will need to close here once readers from
   //other children exist
   close(fds[0][1]);

   while (0 < read(reportFd[0], &statRep, sizeof(Report))) {
      //printf("Last read returned %d\n", status);
      printf("Result from %d, step %d: %0.3lf\n", statRep.id, statRep.step,
       statRep.value);
   }

   //printf("Final read returned %d\n", status);

   close(reportFd[0]);
   
   while ((childPid = waitpid((pid_t) - 1, &status, 0)) != -1) {
      printf("Child %d exits with %d\n", , WEXITSTATUS(status));
   }
/*
   pipe(reportFd);
   if (cells <= 2) {
      if (!fork()) {//child 1
         
      }
      else {
         if (--cells) {//if 2 cells
            if (!fork()) {//child 2
               execv("./Cell", BuildArgList('S', time));
            }
            else {

            }
         }
      }
      close(reportFd[1]);
   }
   else {//normal cases > 2 cells

   }
  */ 
/*
   int i;
   if (cells) {
      int cpid = fork();
      if (cpid < 0) 
         fprintf(stderr, "Bad fork call\n");
      else if (cpid > 0) {    // Parent logic
         close(fda[1]);
      }
      else {                  // Child logic
         // TODO loop to create new cells
         // TODO for each sleep(1) make sure that the reports 
         // reach the parent (binary data) 
         dup2(fda[1], 1);
         close(fda[1]);
         close(fda[0]);

         pipe(fda);
         pipe(fdb)
      }
   }
   wait(NULL);
*/
   
   return 0;
}
