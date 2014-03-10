#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Report.h"

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

#define TWO 2

#define MAX_ARGS 16
#define MAX_STR_SIZE 32

typedef struct fdNode {
   int fd;
   struct fdNode *next, *prev;
} fdNode;

typedef struct closeList {
   int max, size;
   fdNode *head, *tail;
} closeList;

closeList *createCloseList(int max);
void push(closeList *cl, int value);
void mvfdNodes(closeList *src, closeList *dst);
int pop(closeList *cl);
int clearCloseList(closeList *cl);
void deleteCloseList(closeList *cl);
void printCloseList(closeList *c);
char **buildArgs(int numPairs, ...);

int main(int argc, char **argv) {
   double left, right;
   int time, cells = 0; 
   pid_t idMap[MAP_SIZE];

   scanf(" %lf %lf %d %d", &left, &right, &time, &cells);
   if (time < 0)
      time = 0;
   if (cells < 0)
      cells = 0;

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

   pid_t cpid = fork();
   if (cpid < 0)
      fprintf(stderr, "Something forked up\n");
   else if (cpid > 0) { 
      idMap[0] = cpid; 
      clearCloseList(clp);

      for (c = 1; c < cells - 1; c++) {
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

         cpid = fork();
         if (cpid < 0) 
            fprintf(stderr, "Something forked up\n"); 
         else if (cpid > 0) { 
            idMap[c] = cpid; 
            clearCloseList(clp); 
         }
         else {   
            clearCloseList(clc);
            close(fdCellRep[R]);

            if (c == 1)
               execv(CELL_EXEC, buildArgs(ARG_COUNT_INTERIOR,
                'S', time,
                'O', fdLinkRight[TOP_PIPE][W],
                'O', fdCellRep[W],
                'I', fdLinkLeft[TOP_PIPE][R],
                'I', fdLinkRight[BOT_PIPE][R],
                'D', c));
            else if (c == cells - TWO)
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

      for (i = 0; i < 2; i++) {
         fdLinkLeft[TOP_PIPE][i] = fdLinkRight[TOP_PIPE][i];
         fdLinkLeft[BOT_PIPE][i] = fdLinkRight[BOT_PIPE][i];
      }
      cpid = fork();
      if (cpid < 0) 
         fprintf(stderr, "Something forked up\n");
      else if (cpid > 0) { 
         idMap[c] = cpid; 
         close(fdLinkLeft[BOT_PIPE][W]);
         close(fdLinkLeft[TOP_PIPE][R]);
      }
      else { 
         close(fdCellRep[R]);

         if (cells == TWO)
            execv(CELL_EXEC, buildArgs(ARG_COUNT_FIRST - 1, 
             'S', time, 
             'O', fdCellRep[W],
             'V', right, 
             'D', c));
         else
            execv(CELL_EXEC, buildArgs(ARG_COUNT_FIRST, 
             'S', time, 
             'O', fdLinkLeft[BOT_PIPE][W], 
             'O', fdCellRep[W],
             'V', right, 
             'D', c));
      }
   }
   else {   
      clearCloseList(clc);
      close(fdCellRep[R]);

      if (cells == TWO)
         execv(CELL_EXEC, buildArgs(ARG_COUNT_FIRST - 1, 
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

closeList *createCloseList(int max) {
   closeList *cl = malloc(sizeof(closeList));
   cl->max = max;
   cl->size = 0;
   cl->head = cl->tail = NULL;
   return cl;
}

void push(closeList *cl, int value) {
   fdNode *newfd = malloc(sizeof(fdNode));
   newfd->fd = value;
   newfd->next = cl->head;
   newfd->prev = NULL;

   if (cl->size == 0)
      cl->tail = newfd;
   else
      newfd->next->prev = newfd;
   cl->head = newfd;

   if (cl->size == cl->max) {
      cl->tail = cl->tail->prev;
      free(cl->tail->next);
      cl->tail->next = NULL;
      cl->size--;    
   }
   cl->size++;
}

void mvfdNodes(closeList *src, closeList *dst) {
   fdNode *delete = src->head;
   while(delete) {
      push(dst, delete->fd);
      src->head = delete->next;
      free(delete);
      delete = src->head;
      src->size--;
   }
}

int pop(closeList *cl) {
   int rtn = -1;

   fdNode *delete = cl->head;
      if (delete) {
      rtn = delete->fd;
      cl->head = delete->next;
      free(delete);
   }
   return rtn;
}

int clearCloseList(closeList *cl) {
   int rtn = 0;

   fdNode *delete = cl->head;
   while(cl->head) {
      cl->head = delete->next;
      rtn |= close(delete->fd);
      free(delete);
      delete = cl->head;
      cl->size--;
   }
   return rtn;
}

void deleteCloseList(closeList *cl) {
   clearCloseList(cl);
   free(cl);
}

void printCloseList(closeList *cl) {
   fdNode *n = cl->head;

   printf("size: %d | ", cl->size);
   while (n) {
      if (!n->next)
         printf("%d", n->fd);
      else
         printf("%d, ", n->fd);
      n = n->next;
   }
   printf("\n");
}

char **buildArgs(int numPairs, ...) {
   va_list params;
   va_start(params, numPairs);

   int i, c;
   char **args = malloc(MAX_ARGS * sizeof(char *));

   args[0] = "Cell";
   for (i = 1; i <= numPairs; i++) {
      char *str = malloc(MAX_STR_SIZE * sizeof(char));
      c = va_arg(params, int);
      if (c != 'V')
         sprintf(str, "%c%d", c, va_arg(params, int));
      else
         sprintf(str, "%c%0.1lf", c, va_arg(params, double));
      args[i] = str;
   }
   args[i] = NULL;

   return args;
}
