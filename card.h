#ifndef CARD_H
#define CARD_H

#include "dlinked-list.h"

#define NUM_PLAYERS 2

typedef enum { ACT, BUY, END } phase_t;
typedef enum { TURN, DISCARD } force_t;
typedef struct player player_t;
struct player {
  char name[24];
  node_t *deck;
  node_t *hand;
  node_t *discard;  
  node_t *in_play;
  int num_actions;
  int num_buys;
  int num_coins;
  phase_t phase;
  int vp;
  char * (*engine)(player_t *, force_t);
};

typedef enum { VICTORY, TREASURE, ACTION } type_t;
typedef struct card {
  char name[24];
  type_t type;
  unsigned int supply;
  unsigned int cost;
  int vp;
  int tp;
  void (*action)(player_t *, char **);
} card_t;

char * engine_human(player_t *, force_t);

extern int num_empty_piles;

extern card_t copper;
extern card_t silver;
extern card_t gold;
extern card_t curse;
extern card_t estate;
extern card_t duchy;
extern card_t province;

void smithy_action(player_t *, char **);
extern card_t smithy;
void chapel_action(player_t *, char **);
extern card_t chapel;
void village_action(player_t *, char **);
extern card_t village;
void militia_action(player_t *, char **);
extern card_t militia;

extern card_t *card_list[];
extern player_t player_list[NUM_PLAYERS];

/* if the player's deck is empty, it shuffles the discard pile and makes it the deck */
int shuffle(player_t *);

int draw(player_t *);

int gain(player_t *, card_t *);

int buy(player_t *, card_t *);

int trash(player_t *, card_t *);

int play_treasure(player_t *, card_t *);

int play(player_t *, card_t *, char **);

int put_in_play(player_t *, card_t *);

int discard(player_t *, card_t *);

node_t * find(node_t *, card_t *);

/* removes `node` from `src` and adds the card to `dest`; returns 1 if `node` is successfully removed but not added to `dest`, 0 otherwise */
int move(node_t **, node_t **, node_t *);

void move_all(node_t **, node_t **);

card_t * str_to_card(char *);

void node_print(node_t *);

void player_print_debug(player_t *);

void player_print(player_t *, player_t *);

#endif /* CARD_H */
