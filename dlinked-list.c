#include "dlinked-list.h"

char * node_error_repr(node_error_t error) {
  switch (error) {
  case DLINKED_LIST_ERROR_NONE: return "DLINKED_LIST_ERROR_NONE";
  case DLINKED_LIST_ERROR_INVARG: return "DLINKED_LIST_ERROR_INVARG";
  case DLINKED_LIST_ERROR_MEM: return "DLINKED_LIST_ERROR_MEM";
  default: return "";
  }
}

node_error_t node_create(node_t **new, card_t *card) {
  if (*new) { return DLINKED_LIST_ERROR_INVARG; }
  *new = malloc(sizeof(node_t));
  if (!*new) { return DLINKED_LIST_ERROR_MEM; }
  (*new)->card = card;
  (*new)->next = NULL;
  (*new)->prev = NULL;
  return DLINKED_LIST_ERROR_NONE;
}

node_error_t node_preinsert(node_t **current, card_t *card) {
  node_t *new = NULL;
  if (node_create(&new, card)) { return DLINKED_LIST_ERROR_MEM; }
  if (!*current) {
    *current = new;
    return DLINKED_LIST_ERROR_NONE;
  }  
  new->next = *current;
  new->prev = (*current)->prev;
  (*current)->prev = new;
  if (new->prev != NULL) { new->prev->next = new; }
  *current = new;
  return DLINKED_LIST_ERROR_NONE;
}

node_error_t node_insert(node_t *current, card_t *card) {
  node_t *new;
  if (current == NULL) { return DLINKED_LIST_ERROR_INVARG; }
  if (node_create(&new, card)) { return DLINKED_LIST_ERROR_MEM; }
  new->prev = current;
  new->next = current->next;
  current->next = new;
  if (new->next != NULL) { new->next->prev = new; }
  return DLINKED_LIST_ERROR_NONE;
}

node_error_t node_remove(node_t **current) {
  node_t *next;
  if (!*current) { return DLINKED_LIST_ERROR_INVARG; }
  next = (*current)->next;
  if (next) { next->prev = (*current)->prev; }
  if ((*current)->prev) { (*current)->prev->next = next; }
  free(*current);
  *current = next;
  return DLINKED_LIST_ERROR_NONE;
}

size_t node_length(node_t *head) {
  if (head == NULL) { return 0; }
  return node_length(head->next) + 1;
}

size_t node_length_full(node_t *current) {
  if (current == NULL) {return 0; }
  while (current->prev) { current = current->prev; }
  return node_length(current);
}

node_t * node_shuffle(node_t *head) {
  size_t length, i;
  node_t *current, *new = NULL;
  if (head == NULL) { return NULL; }
  /* check to make sure that the head node is really the head */
  while (head->prev != NULL) { head = head->prev; }
  while ((length = node_length(head)) > 0) {
    current = head;
    for (i = random() % length; i > 0; i--) { current = current->next; }
    if (current == head) { head = head->next; }
    if (current->next != NULL) { current->next->prev = current->prev; }
    if (current->prev != NULL) { current->prev->next = current->next; }
    current->next = new;
    current->prev = NULL;
    new = current;
  }
  return new;
}
