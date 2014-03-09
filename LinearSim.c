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

#if DEBUG
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
   push(clp, fdLinkRight[BOT_PIPE][W]);
   push(clc, fdLinkRight[TOP_PIPE][R]);
   push(clc, fdLinkRight[BOT_PIPE][R]);

   // body
   int cpid = fork();
   if (cpid < 0)
      fprintf(stderr, "Something forked up\n");
   else if (cpid > 0) { 
      assert(clearCloseList(clp) == 0);

      // beging creating interior cells
      for (c = 1; c < cells - 1; c++) {
         // setup
         mvfdNodes(clc, clp);

#if DEBUG
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

#if DEBUG
            Report r;
            write(fdCellRep[W], &rThis, sizeof(Report));

            read(fdLinkLeft[TOP_PIPE][R], &r, sizeof(Report));
            fprintf(stderr, "Cell %d read report from left"
             " id: %d, step: %d, value: %lf\n", 
             rThis.id, r.id, r.step, r.value);
             
            // second to last cell does not write to last cell due to fault
            if (rThis.id != cells - 2) 
               write(fdLinkRight[TOP_PIPE][W], &r, sizeof(Report));
            
            read(fdLinkRight[BOT_PIPE][R], &r, sizeof(Report));
            fprintf(stderr, "Cell %d read report from right"
             " id: %d, step: %d, value: %lf\n", 
             rThis.id, r.id, r.step, r.value);
            write(fdLinkLeft[BOT_PIPE][W], &r, sizeof(Report));
#endif

            return 0;
         }
      }

      // begin creating last cell
      for (i = 0; i < 2; i++) {
         fdLinkLeft[TOP_PIPE][i] = fdLinkRight[TOP_PIPE][i];
         fdLinkLeft[BOT_PIPE][i] = fdLinkRight[BOT_PIPE][i];
      }
      assert(close(fdLinkLeft[TOP_PIPE][R]) == 0);
      cpid = fork();
      if (cpid < 0) 
         fprintf(stderr, "Something forked up\n");
      else if (cpid > 0) { 
         assert(close(fdLinkLeft[BOT_PIPE][W]) == 0);
      }
      else {   // last child
         assert(close(fdCellRep[R]) == 0);

#if DEBUG
         Report rThis = { c, c + 100, c + 0.25 }; 
         Report r;
         write(fdCellRep[W], &rThis, sizeof(Report));

         write(fdLinkLeft[BOT_PIPE][W], &rThis, sizeof(Report));
#endif

         return 0;
      }
   }
   else {   // first child 
      assert(clearCloseList(clc) == 0);
      assert(close(fdCellRep[R]) == 0);

#if DEBUG
      Report r = { 0, 100, 100.5 };
      write(fdCellRep[W], &r, sizeof(Report));
      write(fdLinkRight[TOP_PIPE][W], &r, sizeof(Report));
#endif

      return 0;
   }

   close(fdCellRep[W]); 

   // test reading in reports in parent     
#if DEBUG
   int bytes;
   Report r;
   while (bytes = read(fdCellRep[R], &r, sizeof(Report))) {
      fprintf(stdout, "Report r in parent: id %d, step %d, value %lf\n", 
       r.id, r.step, r.value);
   }
#endif

   // parent teardown
   for (i = 0; i < cells; i++) 
      wait(NULL);

   deleteCloseList(clp);
   deleteCloseList(clc);
   return 0;
}
