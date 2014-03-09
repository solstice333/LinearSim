#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Report.h"

#define DEBUG 1
#define MAX_ARGS 8
#define MAX_ARG_SIZE 20

typedef struct idNode {
   int id;
   pid_t pid;
   struct idNode *next;
} idNode;

idNode* AddIdNode(idNode **tail, int id, pid_t pid) {
   if (!*tail) {
      *tail = calloc(1, sizeof(idNode));
      (*tail)->id = id;
      (*tail)->pid = pid;
   }
   else {
      (*tail)->next = calloc(1, sizeof(idNode));
      *tail = (*tail)->next;
      (*tail)->id = id;
      (*tail)->pid = pid;
   }

   return *tail;
}

int RemoveIdNode(idNode **head, pid_t pid) {
   idNode *temp = *head, *prev;
   int oldId = -1;

   while (temp && temp->pid != pid) {
      prev = temp;
      temp = temp->next;
   }

   if (temp) {
      oldId = temp->id;

      if (!prev) {
         *head = temp->next;
      }
      else {
         prev->next = temp->next;
      }
      free(temp);
   }
   else {
      fprintf(stderr, "Error: Trying to delete node that does not exist\n");
   }

   return oldId;
}

/* Builds an argument list that can be passed to execv.
 *
 * numPairs is the number of letter:number pairs that will be created.
 * Following numPairs are all arguments are treated as groups of 2.
 * The first argument in each pair is a char, and any char but 'V'
 * must be followed by an integer. If the char is 'V', follow it with
 * a double. If char is '\0' ignore don't write the following value
 * into argList and lower the total number of values written into argList.
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
      else if (*str != '\0') {
         sprintf(argList[argNdx], "%c%d\0", *str, *(str + sizeof(int)));
         str += sizeof(int) * 2;
      }
      else {
         --argNdx;
         --numPairs;
         str += sizeof(int) * 2;
      }
   }

   argList[argNdx] = NULL;

   return argList;
}

int main(int argc, char **argv) {
   double left, right;
   int time, cells, id, ndx, status;
   int fds[2][2], reportFd[2];
   char **temp, wLeft, wRight;//remove with debug code
   Report statRep;
   pid_t childPid;
   idNode *head, *tail;

   id = 0;
   head = tail = NULL;

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

   temp = BuildArgs(6, 'S', 8, 'F', 10, 'V', 7.2345,
    'T', 123, '\0', 6, 'V', 9.0);
   printf("BuildArgs: %s %s %s %s %s %s %s\n",
    temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6]);
#endif

   pipe(reportFd);
   pipe(fds[0]);

   if (cells > 2) {
      wRight = wLeft = 'O';
   }
   else {
      wRight = wLeft = '\0';
      close(fds[0][1]);
   }

   if (!(childPid = fork())) {
      close(reportFd[0]);
      close(fds[0][0]);


      execv("Cell", BuildArgs(5, 'D', id, 'S', time, 'V', left, 'O',
       reportFd[1], wRight, fds[0][1]));
   }

   close(fds[0][1]);
   head = AddIdNode(&tail, id++, childPid);

   for (; id < cells && childPid; id++) {
      pipe(fds[id % 2]);

      if (id + 1 == cells && !(childPid = fork())) { //its the last cell
         close(reportFd[0]);
         close(fds[id % 2][0]);

         execv("Cell", BuildArgs(5, 'D', id, 'S', time, 'V', right, 'O',
          reportFd[1], wLeft, fds[!(id % 2)][1]));
      }
      else if (!(childPid = fork())) {
         close(reportFd[0]);

         wLeft = wRight = 'O';

         if (id == 1) { //don't try to write to the left
            wLeft = '\0';
            close(fds[!(id % 2)][1]);
         }

         if (id == cells - 2) { //don't try to write to right
            wRight = '\0';
            close(fds[id % 2][1]);
         }
         
         execv("Cell", BuildArgs(7, 'D', id, 'S', time, 'O', reportFd[1],
          wLeft, fds[!(id % 2)][1], wRight, fds[id % 2][1], 'I',
          fds[0][0], 'I', fds[1][0]));
      }
      
      close(fds[!(id % 2)][0]);
      close(fds[!(id % 2)][1]);
      AddIdNode(&tail, id, childPid);
      fprintf(stderr, "id: %d\n", id);
   }
   
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
      printf("Child %d exits with %d\n",
       RemoveIdNode(&head, childPid), WEXITSTATUS(status));
   }
   
   return 0;
}
