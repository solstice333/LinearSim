#ifndef BUILDARGS_H
#define BUILDARGS_H 

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_ARGS 16
#define MAX_STR_SIZE 32

/* Build argument list for execv(). |numPairs| represents the letter:number
*  pairs. |...| should be replaced with the letter:number pairs, for example,
*  buildArgs(4, 'S', 5, 'O', 6, 'V', 42.5, 'D', 0) contains 4 pairs and will
*  return output: { "Cell", "S5", "O6", "V42.5", "D0" }. The number associated
*  to 'V' must be a double. All the rest are of type int. Any other usage
*  not as described, is undefined behavior.
*/
char **buildArgs(int numPairs, ...);

#endif
