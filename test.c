#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

int main() {
   int fd[2];
   pipe(fd);

   fprintf(stderr, "fd[0]: %d\n", fd[0]);
   fprintf(stderr, "fd[1]: %d\n", fd[1]);
   //fprintf(stderr, "close: %d\n", close(3));
}
