
#include "player.h"

/*	generates a random int in range [0,n) according to a uniform distribution  */
int rand_int(int n) {
	if ((n - 1) == RAND_MAX)
		return rand();
	else {
		long end = RAND_MAX / n;
		end *= n;
		int r;
		while ((r = rand()) >= end);
		return r % n;
	}
}

/*  standard actions  */
int count(card_array_t * arr, type_e ref_type) {
	unsigned int count = 0;
	unsigned int i;
	for (i = 0; i < arr->size; i++) {
		if (card_array_get(arr, i)->type == ref_type)  //will need to change this to account for card with several types e.g. victory-action
			++count;
	}
	return count;
}
int shuffle(player_t * p) {
	int size;
	for (size = p->discards.size; size > 0; size--) {
		int i = rand_int(size);
		card_array_transfer(&p->discards, i, &p->deck, p->deck.size);
	}
	return 0;
}
int draw(player_t * p) {
	if (p->deck.size == 0)
		shuffle(p);
	if (p->deck.size == 0)
		return 0;
	int idx = p->deck.size - 1;
	return card_array_transfer(&p->deck, idx, &p->hand, p->hand.size);
}
int play(player_t * p, card_t * c, ...) {
	int ret = place(&p->hand, c, &p->in_play);
	if (ret == -1)
		return -1;
	
	tp_pool += c->tp;
	
	va_list val;
	va_start(val, c);
	ret = c->act(val);
	va_end(val);
	return ret;
}
int place(card_array_t * org, card_t * c, card_array_t * dest) {
	int idx = card_array_find(org, c);
	if (idx == -1)
		return -1;
	return card_array_transfer(org, idx, dest, dest->size);
}
int discard(player_t * p, card_array_t * org, card_t * c) {
	return place(org, c, &p->discards);
}
int trash(card_array_t * org, card_t * c) {
	int idx = card_array_find(org, c);
	if (idx == -1)
		return -1;
	card_array_remove(org, idx);
	return 0;
}
int gain(card_array_t * dest, card_t * c) {
	if (c->supply == 0)
		return -1;
	
	if (--(c->supply) == 0)
		empty_pile_num++;
	
	card_array_append(dest, c);
	return 0;
}
int buy(card_array_t * dest, card_t * c) {
	if (c->cost > tp_pool)
		return -1;
	tp_pool -= c->cost;
	return gain(dest, c);
}

/*	nonstandard actions  */
void establish_basic_kingdom(void) {
	card_array_init(&kingdom, 10, strictest);
	
	card_array_append(&kingdom, &curse);
	card_array_append(&kingdom, &estate);
	card_array_append(&kingdom, &duchy);
	card_array_append(&kingdom, &province);
	card_array_append(&kingdom, &colony);
	card_array_append(&kingdom, &copper);
	card_array_append(&kingdom, &silver);
	card_array_append(&kingdom, &gold);
	card_array_append(&kingdom, &platinum);
}
void establish_pgnbk(void) {
	#define pgnbk pre_game_non_basic_kingdom
	card_array_init(&pgnbk, 10, loose);
	
	card_array_append(&pgnbk, &artisan);
	card_array_append(&pgnbk, &smithy);
	card_array_append(&pgnbk, &chapel);
}
int start_game(void) {
	int i, j;
	
	/*  establish basic kingdom supply and add ten non-basic kingdom cards  */
	establish_basic_kingdom();
	establish_pgnbk();
	#define pgnbk pre_game_non_basic_kingdom
	for (i = 0; i < KINGDOM_NUM; i++) {
		if (pgnbk.size == 0)
			return -1;
		j = rand_int(pgnbk.size);
		card_array_transfer(&pgnbk, j, &kingdom, 0);
	}
	card_array_free(&pgnbk);
	/*	correct supply pile numbers  */
	if (PLAYER_NUM == 3)
		curse.supply += 10;
		copper.supply -= 7;
		estate.supply += 4, duchy.supply += 4, province.supply += 4, colony.supply += 4;
	if (PLAYER_NUM == 4) {
		curse.supply += 10;
		copper.supply -= 7;
	}
	if (!CP_ACTIVE) {
		colony.supply = 0;
		platinum.supply = 0;
	}

	/*	initialize semi-permanent status variables  */
	empty_pile_num = 0;
	trade_route_pool = 0;

	/*	initialize transient status variables  */
	current_player_num = rand_int(PLAYER_NUM);
	current_player = player + current_player_num;
	action_num = 1;
	buy_num = 1;
	tp_pool = 0;
	
	/*	set up each player's deck in turn  */
	for (i = 0; i < PLAYER_NUM; i++) {
		player_t * p = player + i;
		p->name[0] = 'a' + i;	//temporary
		int ret = 0;
		ret += card_array_init(&p->deck, 10, strict);
		ret += card_array_init(&p->hand, 10, loose);
		ret += card_array_init(&p->in_play, 10, loose);
		ret += card_array_init(&p->discards, 10, loose);
		for (j = 0; j < 7; j++)
			ret += card_array_append(&p->discards, &copper);
		for (j = 0; j < 3; j++)
			ret += card_array_append(&p->discards, &estate);
		ret += shuffle(p);
		for (j = 0; j < 5; j++)
			ret += draw(p);
		if (ret < 0)
			return -1;
	}
	return 0;
}
