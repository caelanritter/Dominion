
#ifndef LIBRARY_H
#define LIBRARY_H

#include "card.h"
#include "player.h"

/*	contains a single pointer to every type of non-basic kingdom card; ten of these are added to card_array_t kingdom for the actual game's supply  */
card_array_t pre_game_non_basic_kingdom;
/*	contains a single pointer to every type of card in the supply, to make indexing uniform  */
card_array_t kingdom;

/*  basic cards  */
card_t curse, estate, duchy, province, colony;
card_t copper, silver, gold, platinum;

/*  ------------kingdom cards------------  */

/*	artisan
 *	gains a card to hand costing up to five; puts a card from hand on top of deck
 *	expects player_t *; expects card_t * of which card to gain, then card_t * of which card to put on deck  */
card_t artisan;
int artisan_act(va_list);

/*  smithy
 *	draws three cards
 *	expects player_t *; expects no further arguments  */
card_t smithy;
int smithy_act(va_list);

/*  chapel
 *	trashes up to four cards from hand
 *	expects player_t *; expects an unsigned int containing the number of card_t *'s to trash, followed by each of the card_t *'s  */
card_t chapel;
int chapel_act(va_list);

#endif	/*  LIBRARY_H  */
