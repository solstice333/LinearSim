#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "Report.h"

#define MAX_FD 12
#define END_EXIT_STATUS 42

int main(int argc, char** argv) {
   char *temp;
   int readers[2], writers[MAX_FD], maxTime, uId, isFixed, iTemp,
    numRdrs, numWrtrs, curStep, ndxWrtrs;
   double value = 0;
   Report status;

   maxTime = isFixed = numRdrs = numWrtrs = 0;
   
   while (temp = *argv++) {
      switch (*temp++) {
      case 'S':
         sscanf(temp, "%d", &maxTime);
         break;
      case 'O':
         sscanf(temp, "%d", &iTemp);
         if (MAX_FD >= iTemp) {
            writers[numWrtrs++] = iTemp;
         }
         break;
      case 'I':
         sscanf(temp, "%d", &iTemp);
         if (MAX_FD >= iTemp) {
            readers[numRdrs++] = iTemp;
         }
         break;
      case 'V':
         isFixed++;
         sscanf(temp, "%lf", &value);
         break;
      case 'D':
         sscanf(temp, "%d", &uId);
         break;
      }
   }

   for (curStep = 0; curStep <= maxTime; curStep++) {
      status.id = uId;
      status.step = curStep;
      status.value = value;

      for (ndxWrtrs = 0; ndxWrtrs < numWrtrs; ndxWrtrs++) {
         write(writers[ndxWrtrs], &status, sizeof(Report));
      }

      if (!isFixed) {
         read(readers[0], &status, sizeof(Report));
         value = status.value;
         read(readers[1], &status, sizeof(Report));
         value = (value + status.value) / 2;
      }

   }
   
   for (ndxWrtrs = 0; ndxWrtrs < numWrtrs; ndxWrtrs++) {
      close(writers[ndxWrtrs]);
   }

   if (!isFixed) {
      close(readers[0]);
      close(readers[1]);
      exit(0);
   }

   exit(END_EXIT_STATUS);
}
