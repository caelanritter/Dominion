#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "card.h"

int num_empty_piles = 0;

card_t copper = { "copper", TREASURE, 60, 0, 0, 1, NULL };
card_t silver = { "silver", TREASURE, 40, 3, 0, 2, NULL };
card_t gold = { "gold", TREASURE, 30, 6, 0, 3, NULL };
card_t curse = { "curse", VICTORY, 30, 0, -1, 0, NULL };
card_t estate = { "estate", VICTORY, 24, 2, 1, 0, NULL };
card_t duchy = { "duchy", VICTORY, 12, 5, 3, 0, NULL };
card_t province = { "province", VICTORY, 12, 8, 6, 0, NULL };

void smithy_action(player_t *player, char **arg) {
  int i;
  for (i = 0; i < 3; i++) { draw(player); }
}
card_t smithy = { "smithy", ACTION, 10, 4, 0, 0, smithy_action };

void chapel_action(player_t *player, char **arg) {
  int i;
  for (i = 0; i < 4; i++) {
    if (!arg[i]) { break; }
    trash(player, str_to_card(arg[i]));
  }
}
card_t chapel = { "chapel", ACTION, 10, 2, 0, 0, chapel_action };

void village_action(player_t *player, char **arg) {
  draw(player);
  player->num_actions += 2;
}
card_t village = { "village", ACTION, 10, 3, 0, 0, village_action };

void militia_action(player_t *player, char **arg) {
  int i;
  player->num_coins+= 2;
  for (i = 0; i < NUM_PLAYERS; i++) {
    player_t * player1 = &player_list[i];
    if (player1->phase == END) {
      if (player1->engine == engine_human) { player_print(player1, player1); }
      while (node_length_full(player1->hand) > 3) {
	printf("discard ");
	char *command = player1->engine(player1, DISCARD);
	printf("%s\n", command);
	card_t *card = str_to_card(command);
	if (!card) { continue; }
	discard(player1, card);
      }
    }
  }
}
card_t militia = { "militia", ACTION, 10, 4, 0, 0, militia_action };

card_t *card_list[] = { &copper, &silver, &gold, &curse, &estate, &duchy, &province, &smithy, &chapel, &village, &militia };
player_t player_list[NUM_PLAYERS];

int shuffle(player_t *player) {
  if (player->deck) { return 1; }
  player->deck = node_shuffle(player->discard);
  player->discard = NULL;
  return 0;
}

int draw(player_t *player) {
  if (!player->deck) { shuffle(player); }
  if (!player->deck) { return 1; }
  if (node_preinsert(&player->hand, player->deck->card)) { exit(EXIT_FAILURE); }
  node_remove(&player->deck);
  return 0;
}

int gain(player_t *player, card_t *card) {
  if (card->supply == 0) { return 1; }
  card->supply--;
  player->vp += card->vp;
  if (card->supply == 0) { num_empty_piles++; }
  if (node_preinsert(&player->discard, card)) { exit(EXIT_FAILURE); }
  return 0;
}

int buy(player_t *player, card_t *card) {
  if (player->num_buys == 0 || player->phase != BUY || player->num_coins < card->cost || card->supply == 0) { return 1; }
  if (gain(player, card)) { return 1; }
  player->num_buys--;
  player->num_coins -= card->cost;
  return 0;
}

int trash(player_t *player, card_t *card) {
  node_t *current = player->hand;
  if (!current) { return 1; }
  while (current->prev) { current = current->prev; }
  while (current) {
    if (current->card == card) {
      if (current == player->hand) { node_remove(&player->hand); }
      else { node_remove(&current); }
      player->vp -= card->vp;
      return 0;
    }
    current = current->next;
  }
  return 1;
}

/* int play_action(player_t *player, card_t *card) { */
/*   node_t *current = player->hand; */
/*   if (player->num_actions == 0 || player->phase != ACT || card->type != ACTION) { return 1; } */
/*   while (current) { */
/*     if (current->card == card) { */
/*       if (current == player->hand) { node_remove(&player->hand); } */
/*       else { node_remove(&current); } */
/*       if (node_preinsert(&player->in_play, card)) { exit(EXIT_FAILURE); } */
/*       player->num_actions--; */
/*       card->action(player); */
/*       return 0; */
/*     } */
/*     current = current->next; */
/*   } */
/*   return 1; */
/* } */

int play_treasure(player_t *player, card_t *card) {
  node_t *current = player->hand;
  if (player->phase != BUY || card->type != TREASURE) { return 1; }
  while (current) {
    if (current->card == card) {
      if (current == player->hand) { node_remove(&player->hand); }
      else { node_remove(&current); }
      if (node_preinsert(&player->in_play, card)) { exit(EXIT_FAILURE); }
      player->num_coins += card->tp;
      return 0;
    }
    current = current->next;
  }
  return 1;
}

int play(player_t *player, card_t *card, char **arg) {
  node_t *current = player->hand;
  if (!current) { return 1; }
  while (current->prev) { current = current->prev; }
  while (current) {
    if (current->card == card) {
      if (current == player->hand) { node_remove(&player->hand); }
      else { node_remove(&current); }
      if (node_preinsert(&player->in_play, card)) { exit(EXIT_FAILURE); }
      player->num_coins += card->tp;
      if (card->action) { card->action(player, arg); }
      return 0;
    }
    current = current->next;
  }
  return 1;
}

int put_in_play(player_t *player, card_t *card) {
  node_t *current = player->hand;
  if (!current) { return 1; }
  while (current->prev) { current = current->prev; }
  while (current) {
    if (current->card == card) {
      if (current == player->hand) { node_remove(&player->hand); }
      else { node_remove(&current); }
      if (node_preinsert(&player->in_play, card)) { exit(EXIT_FAILURE); }
      return 0;
    }
    current = current->next;
  }
  return 1;
}

int discard(player_t *player, card_t *card) {
  node_t *current = player->hand;
  if (!current) { return 1; }
  while (current->prev) { current = current->prev; }
  while (current) {
    if (current->card == card) {
      if (current == player->hand) { node_remove(&player->hand); }
      else { node_remove(&current); }
      if (node_preinsert(&player->discard, card)) { exit(EXIT_FAILURE); }
      return 0;
    }
    current = current->next;
  }
  return 1;
}

node_t *find(node_t *current, card_t *card) {
  if (!current) { return NULL; }
  while (current->prev) { current = current->prev; }
  while (current) {
    if (current->card == card) { return current; }
    current = current->next;
  }
  return NULL;
}

int move(node_t **src, node_t **dest, node_t *node) {
  card_t *card;
  if (!src || !node) { return 0; }
  card = node->card;
  if (node == *src) { node_remove(src); }
  else { node_remove(&node); }
  if (!dest) { return 1; }
  if (node_preinsert(dest, card)) { exit(EXIT_FAILURE); }
  return 0;
}

void move_all(node_t **src, node_t **dest) {
  node_t *tail = *src, *head;
  if (*src == NULL) { return; }
  if (*dest == NULL) {
    *dest = *src;
    *src = NULL;
    return;
  }
  while ((*src)->prev != NULL) { *src = (*src)->prev; }
  while ((*dest)->prev != NULL) { *dest = (*dest)->prev; }
  head = *dest;
  while (tail->next != NULL) { tail = tail->next; }
  while (head->prev != NULL) { head = head->prev; }
  tail->next = head;
  head->prev = tail;
  *dest = *src;
  *src = NULL;
}

card_t * str_to_card(char *str) {
  int i;
  for (i = 0; i < sizeof(card_list) / sizeof(card_list[0]); i++) {
    if (!strcmp(str, card_list[i]->name)) { return card_list[i]; }
  }
  return NULL;
}

void node_print(node_t *current) {
  if (current == NULL) { printf("\n"); return; }
  while (current->prev != NULL) { current = current->prev; }
  while (current != NULL) {
    printf("%s ", current->card->name);
    current = current->next;
  }
  printf("\n");
}

// DEBUG
void player_print_debug(player_t *player) {
  char *phase;
  switch (player->phase) {
  case ACT: phase = "act"; break;
  case BUY: phase = "buy"; break;
  case END: phase = "end"; break;
  }
  printf("%s (%d/%d/%d/%s)\n", player->name, player->num_actions, player->num_buys, player->num_coins, phase);
  printf("DECK: ");
  node_print(player->deck);
  printf("HAND: ");
  node_print(player->hand);
  printf("IN PLAY: ");
  node_print(player->in_play);
  printf("DISCARD: ");
  node_print(player->discard);
  printf("\n");
}

void player_print(player_t *player, player_t *requester) {
  char *phase;
  switch (player->phase) {
  case ACT: phase = "act"; break;
  case BUY: phase = "buy"; break;
  case END: phase = "end"; break;
  }
  printf("%s (%d/%d/%d/%s)\n", player->name, player->num_actions, player->num_buys, player->num_coins, phase);
  printf("DECK: ");
  if (!requester) { node_print(player->deck); }
  else if (requester == player) { printf("%ld cards\n", node_length(player->deck)); }
  else { printf("%sempty\n", player->deck ? "non" : ""); }
  printf("HAND: ");
  if (!requester || requester == player) { node_print(player->hand); }
  else { printf("%ld cards\n", node_length(player->hand)); }
  printf("IN PLAY: ");
  node_print(player->in_play);
  printf("DISCARD: ");
  if (player->discard != NULL) { printf("%s on top", player->discard->card->name); }
  printf("\n\n");
}
