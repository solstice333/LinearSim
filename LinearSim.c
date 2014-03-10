#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Report.h"
#include "CloseList.h"
#include "BuildArgs.h"

#define MAP_SIZE 256

#define CELL_LINK 2
#define TOP_PIPE 0
#define BOT_PIPE 1
#define CLOSE_PARENT 4
#define CLOSE_CHILD 2
#define R 0
#define W 1

#define CELL_EXEC "./Cell"
#define ARG_COUNT_FIRST 5
#define ARG_COUNT_INTERIOR 6
#define ARG_COUNT_LAST 5

#define HACKED 2

int main(int argc, char **argv) {
   double left, right;
   int time, cells = 0; 
   pid_t idMap[MAP_SIZE];

   scanf(" %lf %lf %d %d", &left, &right, &time, &cells);
   if (time < 0)
      time = 0;
   if (cells < 0)
      cells = 0;

   // setup parent
   int i, c;
   int fdCellRep[2];
   int fdLinkRight[CELL_LINK][2];
   int fdLinkLeft[CELL_LINK][2];
   closeList *clp = createCloseList(CLOSE_PARENT);
   closeList *clc = createCloseList(CLOSE_CHILD);
   
   pipe(fdCellRep);
   pipe(fdLinkRight[TOP_PIPE]);
   pipe(fdLinkRight[BOT_PIPE]);

   push(clp, fdLinkRight[TOP_PIPE][W]);
   push(clp, fdLinkRight[BOT_PIPE][R]);
   push(clc, fdLinkRight[TOP_PIPE][R]);
   push(clc, fdLinkRight[BOT_PIPE][W]);

   // body
   pid_t cpid = fork();
   if (cpid < 0)
      fprintf(stderr, "Something forked up\n");
   else if (cpid > 0) { 
      idMap[0] = cpid; // cpid for first child
      assert(clearCloseList(clp) == 0);

      // begin creating interior cells
      for (c = 1; c < cells - 1; c++) {
         // setup
         mvfdNodes(clc, clp);

         for (i = 0; i < 2; i++) {
            fdLinkLeft[TOP_PIPE][i] = fdLinkRight[TOP_PIPE][i];
            fdLinkLeft[BOT_PIPE][i] = fdLinkRight[BOT_PIPE][i];
         }
         pipe(fdLinkRight[TOP_PIPE]);
         pipe(fdLinkRight[BOT_PIPE]);

         push(clp, fdLinkRight[TOP_PIPE][W]);
         push(clp, fdLinkRight[BOT_PIPE][R]);
         push(clc, fdLinkRight[TOP_PIPE][R]);
         push(clc, fdLinkRight[BOT_PIPE][W]);

         // body
         cpid = fork();
         if (cpid < 0) 
            fprintf(stderr, "Something forked up\n"); 
         else if (cpid > 0) { 
            idMap[c] = cpid; // cpid for interior children
            assert(clearCloseList(clp) == 0); 
         }
         else {   // interior child   
            assert(clearCloseList(clc) == 0);
            assert(close(fdCellRep[R]) == 0);

            if (c == 1)
               execv(CELL_EXEC, buildArgs(ARG_COUNT_INTERIOR,
                'S', time,
                'O', fdLinkRight[TOP_PIPE][W],
                'O', fdCellRep[W],
                'I', fdLinkLeft[TOP_PIPE][R],
                'I', fdLinkRight[BOT_PIPE][R],
                'D', c));
            else if (c == cells - 2)
               execv(CELL_EXEC, buildArgs(ARG_COUNT_INTERIOR,
                'S', time,
                'O', fdLinkLeft[BOT_PIPE][W],
                'O', fdCellRep[W],
                'I', fdLinkLeft[TOP_PIPE][R],
                'I', fdLinkRight[BOT_PIPE][R],
                'D', c));
            else 
               execv(CELL_EXEC, buildArgs(ARG_COUNT_INTERIOR + 1,
                'S', time,
                'O', fdLinkLeft[BOT_PIPE][W],
                'O', fdLinkRight[TOP_PIPE][W],
                'O', fdCellRep[W],
                'I', fdLinkLeft[TOP_PIPE][R],
                'I', fdLinkRight[BOT_PIPE][R],
                'D', c));
         }
      }

      // begin creating last cell
      for (i = 0; i < 2; i++) {
         fdLinkLeft[TOP_PIPE][i] = fdLinkRight[TOP_PIPE][i];
         fdLinkLeft[BOT_PIPE][i] = fdLinkRight[BOT_PIPE][i];
      }
      cpid = fork();
      if (cpid < 0) 
         fprintf(stderr, "Something forked up\n");
      else if (cpid > 0) { 
         idMap[c] = cpid; // cpid for last child
         assert(close(fdLinkLeft[BOT_PIPE][W]) == 0);
         assert(close(fdLinkLeft[TOP_PIPE][R]) == 0);
      }
      else {   // last child
         assert(close(fdCellRep[R]) == 0);

         execv(CELL_EXEC, buildArgs(ARG_COUNT_FIRST, 
          'S', time, 
          'O', fdLinkLeft[BOT_PIPE][W], 
          'O', fdCellRep[W],
          'V', right, 
          'D', c));
      }
   }
   else {   // first child 
      assert(clearCloseList(clc) == 0);
      assert(close(fdCellRep[R]) == 0);

      if (cells == HACKED)
         execv(CELL_EXEC, buildArgs(ARG_COUNT_FIRST, 
          'S', time, 
          'O', fdCellRep[W],
          'V', left, 
          'D', 0));
      else
         execv(CELL_EXEC, buildArgs(ARG_COUNT_FIRST, 
          'S', time, 
          'O', fdLinkRight[TOP_PIPE][W], 
          'O', fdCellRep[W],
          'V', left, 
          'D', 0));
   }

   // parent teardown
   close(fdCellRep[W]); 

   Report r;
   if (cells != 0) {
      while (read(fdCellRep[R], &r, sizeof(Report))) {
         if (time != 0)
            printf("Result from %d, step %d: %0.3lf\n", 
             r.id, r.step, r.value);

         if (r.step == time) {
            int status;
            cpid = wait(&status);

            for (i = 0; i < cells; i++) 
               if (cpid == idMap[i]) 
                  printf("Child %d exits with %d\n", 
                   i, WEXITSTATUS(status));
         }
      }
   }

   deleteCloseList(clp);
   deleteCloseList(clc);
   return 0;
}
