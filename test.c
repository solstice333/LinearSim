#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "BuildArgs.h" 

int main() {
   char **args = buildArgs(4, 'S', 5, 'O', 6, 'V', 42.5, 'D', 0);

   while (*args) 
      printf("%s ", *args++);
   printf("\n");

   args = buildArgs(6, 'S', 5, 'O', 3, 'O', 4, 'I', 5, 'I', 6, 'D', 1);
   while (*args) 
      printf("%s ", *args++);
   printf("\n");

   args = buildArgs(1, 'S', 5);
   while (*args) 
      printf("%s ", *args++);
   printf("\n");

   args = buildArgs(0, 'S', 5);
   while (*args) 
      printf("%s ", *args++);
   printf("\n");

   return 0;
}
