#include "CloseList.h"

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
