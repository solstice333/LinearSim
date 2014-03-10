#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Report.h"
#include "CloseList.h"
#include "BuildArgs.h"

#define DEBUG_PIPES 0
#define DEBUG_EXEC 1

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

int main(int argc, char **argv) {
   double left, right;
   int time, cells = 0; 
   int id = 0;
   int reportfd[2]; 

   scanf(" %lf %lf %d %d", &left, &right, &time, &cells);
   if (time < 0)
      time = 0;
   if (cells < 0)
      cells = 0;

#if DEBUG_PIPES
   fprintf(stderr, "left: %0.1lf\n", left);
   fprintf(stderr, "right: %0.1lf\n", right);
   fprintf(stderr, "time: %d\n", time);
   fprintf(stderr, "cells: %d\n", cells);
#endif

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
   int cpid = fork();
   if (cpid < 0)
      fprintf(stderr, "Something forked up\n");
   else if (cpid > 0) { 
      assert(clearCloseList(clp) == 0);

      // begin creating interior cells
      for (c = 1; c < cells - 1; c++) {
         // setup
         mvfdNodes(clc, clp);

#if DEBUG_PIPES
         Report rThis = { c, c + 100, c + 0.25 };          
#endif

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
         else if (cpid > 0) 
            assert(clearCloseList(clp) == 0); 
         else {   // interior child   
            assert(clearCloseList(clc) == 0);
            assert(close(fdCellRep[R]) == 0);

#if DEBUG_PIPES
            Report r;
            write(fdCellRep[W], &rThis, sizeof(Report));

            read(fdLinkLeft[TOP_PIPE][R], &r, sizeof(Report));
            fprintf(stderr, "Cell %d read report from left"
             " id: %d, step: %d, value: %lf\n", 
             rThis.id, r.id, r.step, r.value);

            write(fdLinkRight[TOP_PIPE][W], &r, sizeof(Report));
             
            read(fdLinkRight[BOT_PIPE][R], &r, sizeof(Report));
            fprintf(stderr, "Cell %d read report from right"
             " id: %d, step: %d, value: %lf\n", 
             rThis.id, r.id, r.step, r.value);
            write(fdLinkLeft[BOT_PIPE][W], &r, sizeof(Report));
#elif DEBUG_EXEC
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
#endif
            return 0;
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
         assert(close(fdLinkLeft[BOT_PIPE][W]) == 0);
         assert(close(fdLinkLeft[TOP_PIPE][R]) == 0);
      }
      else {   // last child
         assert(close(fdCellRep[R]) == 0);

#if DEBUG_PIPES
         Report rThis = { c, c + 100, c + 0.25 }; 
         Report r;
         write(fdCellRep[W], &rThis, sizeof(Report));

         // this line is to make sure the reader on this end doesn't close 
         // before the left cell writes to the pipe
         read(fdLinkLeft[TOP_PIPE][R], &r, sizeof(Report));

         write(fdLinkLeft[BOT_PIPE][W], &rThis, sizeof(Report));
#elif DEBUG_EXEC
         execv(CELL_EXEC, buildArgs(ARG_COUNT_FIRST, 
          'S', time, 
          'O', fdLinkLeft[BOT_PIPE][W], 
          'O', fdCellRep[W],
          'V', right, 
          'D', c));
#endif

         return 0;
      }
   }
   else {   // first child 
      assert(clearCloseList(clc) == 0);
      assert(close(fdCellRep[R]) == 0);

#if DEBUG_PIPES
      Report r = { 0, 100, 100.5 };
      write(fdCellRep[W], &r, sizeof(Report));
      write(fdLinkRight[TOP_PIPE][W], &r, sizeof(Report));
#elif DEBUG_EXEC
      execv(CELL_EXEC, buildArgs(ARG_COUNT_FIRST, 
       'S', time, 
       'O', fdLinkRight[TOP_PIPE][W], 
       'O', fdCellRep[W],
       'V', left, 
       'D', 0));
#endif

      return 0;
   }

   // parent teardown
   close(fdCellRep[W]); 

#if DEBUG_EXEC || DEBUG_PIPES
   int status;
   Report r;
   while (read(fdCellRep[R], &r, sizeof(Report))) {
      fprintf(stdout, "Result from %d, step %d: %0.3lf\n",
       r.id, r.step, r.value);

      if (r.step == time) {
         wait(&status);
         fprintf(stdout, "Child %d exits with %d\n", 
          r.id, WEXITSTATUS(status));
      }
   }
#endif

   deleteCloseList(clp);
   deleteCloseList(clc);
   return 0;
}
