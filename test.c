#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Report.h"
#include "CloseList.h"

#define DEBUG 1

#define CELL_LINK 2
#define TOP_PIPE 0
#define BOT_PIPE 1
#define CLOSE_PARENT 4
#define CLOSE_CHILD 2
#define R 0
#define W 1

int main() {
   int fd[2][2];
   closeList *clp = createCloseList(CLOSE_PARENT);
   closeList *clc = createCloseList(CLOSE_CHILD);

   pipe(fd[0]);
   pipe(fd[1]);

   addfdNode(clp, fd[0][W]);
   addfdNode(clp, fd[1][W]);
   addfdNode(clc, fd[0][R]);
   addfdNode(clc, fd[1][R]);

   printf("clp: ");
   printCloseList(clp);

   printf("clc: ");
   printCloseList(clc);

   int cpid = fork();
   if (cpid < 0) {
      fprintf(stderr, "Something went wrong forking...\n");
      return 1;
   }
   if (cpid > 0) {
      int readVal;
      assert(clearCloseList(clp) == 0);

      assert(close(fd[0][W]) == -1);
      assert(close(fd[1][W]) == -1);

      read(fd[0][R], &readVal, sizeof(int));
      fprintf(stderr, "Read value %d\n", readVal);

      printf("clp: ");
      printCloseList(clp);
      printf("clc: ");
      printCloseList(clc);

      addfdNode(clp, 4);
      addfdNode(clp, 6);

      printf("clp: ");
      printCloseList(clp);
      assert(clearCloseList(clp) == -1);
      printf("clp: ");
      printCloseList(clp);
   }
   else {
      int writeVal = 100;
      clearCloseList(clc);
      write(fd[0][W], &writeVal, sizeof(int));
      return 0;
   }

   wait(NULL);
   deleteCloseList(clp);
   deleteCloseList(clc);

   return 0;
}
