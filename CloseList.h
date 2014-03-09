#ifndef CLOSELIST_H
#define CLOSELIST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* The closeList operates such that when |size| equals |max|, the last value
*  at the tail is dropped. For example if the max size of the list is 4 and
*  we add an fdNode to it, the fdNode at the tail will get dropped, while
*  the new fdNode will get added. The functionality is somewhat similar to
*  a queue in which an enqueue and dequeue happen simultaneously
*/

/* The fdNode is a wrapper for an open fd that needs to be closed to be 
*  added to the closeList 
*/
typedef struct fdNode {
   int fd;
   struct fdNode *next, *prev;
} fdNode;

/* closeList contains the linked list of fd's to be closed */
typedef struct closeList {
   int max, size;
   fdNode *head, *tail;
} closeList;

/* Creates a closeList containing a maximum number of fd nodes |max| 
*  Any addition of fdNode objects will force the tail fdNode to be
*  dropped
*/
closeList *createCloseList(int max);

/* Adds an fd of |value| to the closeList |cl| */
void push(closeList *cl, int value);

/* Moves the nodes from |src| to |dst| */
void mvfdNodes(closeList *src, closeList *dst);

/* Pops the fd at head from |cl| and returns its fd value. 
*  Returns -1 if |cl| is empty
 */
int pop(closeList *cl);

/* Closes all of the fd's stored inside closeList and prepares the 
*  closeList |cl| for additional fd's to be closed. Returns -1 if
*  close failed for any fd, otherwise returns 0 on success for all
*/
int clearCloseList(closeList *cl);

/* Teardown for the closeList. Frees up allocated space */
void deleteCloseList(closeList *cl);

/* Print the size and the contents of the closeList */
void printCloseList(closeList *c);

#endif
