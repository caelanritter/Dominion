#ifndef DLINKED_LIST_H
#define DLINKED_LIST_H

#include <stdlib.h>

typedef struct card card_t;
typedef struct node node_t;
struct node {
  card_t *card;
  node_t *next;
  node_t *prev;
};

typedef enum {
  DLINKED_LIST_ERROR_NONE = 0,
  DLINKED_LIST_ERROR_INVARG,
  DLINKED_LIST_ERROR_MEM,
} node_error_t;

/* returns a string representation of an error value */
char * node_error_repr(node_error_t);

/* accepts a pointer to a null pointer and points it at a single node with the given data */
node_error_t node_create(node_t **, card_t *);

/* inserts a node with the given data immediately before the current node, and shifts the pointer back one step */
node_error_t node_preinsert(node_t **, card_t *);

/* inserts a node with the given data immediately after the current node */
node_error_t node_insert(node_t *, card_t *);

/* removes the current node points it at the next node */
node_error_t node_remove(node_t **);

/* returns the length of a list starting at the given node */
size_t node_length(node_t *);

/* returns the full length of a list */
size_t node_length_full(node_t *);

/* shuffles the list and returns a pointer to the head node */
node_t * node_shuffle(node_t *);

#endif /* DLINKED_LIST_H */
