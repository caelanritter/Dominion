#ifndef PLAYER_H
#define PLAYER_H

#include "library.h"

#define PLAYER_NUM 4	//number of players in the game
#define CP_ACTIVE 0		//colonies and platinums in the game? (boolean)
#define KINGDOM_NUM 3	//should be 10 once that many nonbasic cards are implemented

struct player_t {
	char name[24];
	card_array_t deck;
	card_array_t hand;
	card_array_t in_play;
	card_array_t discards;
	unsigned int vp_token;
};

player_t player[PLAYER_NUM];

/*  transient status variables  */
unsigned int current_player_num;
player_t * current_player;
unsigned int action_num;
unsigned int buy_num;
unsigned int tp_pool;

/*	semi-permanent status variables  */
unsigned int empty_pile_num;
unsigned int trade_route_pool;

/*  returns random integer uniformly  */
int rand_int(int);

/*  standard actions  */
int count(card_array_t *, type_e);
int draw(player_t *);
int shuffle(player_t *);
int play(player_t *, card_t *, ...);
int place(card_array_t *, card_t *, card_array_t *);
int discard(player_t *, card_array_t *, card_t *);
int trash(card_array_t *, card_t *);
int gain(card_array_t *, card_t *);
int buy(card_array_t *, card_t *);

/*	nonstandard actions  */
void establish_basic_kingdom(void);
void establish_pgnbk(void);
int start_game(void);

#endif	/*  PLAYER_H  */