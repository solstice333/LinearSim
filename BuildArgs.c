#include "BuildArgs.h"

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
