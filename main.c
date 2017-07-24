
#include <time.h>

#include "library.h"
#include "player.h"

/* 	debug functions  */
void print_deck(player_t * p) {
	signed int i;
	printf("[deck]\t");
for (i = p->deck.size - 1; i >= 0; i--) {
		card_t * item =	card_array_get(&p->deck, i);
		printf("%s ", item->name);
	}
	printf("\n");
}
void print_hand(player_t * p) {
	unsigned int i;
	printf("[hand]\t");
for (i = 0; i < p->hand.size; i++) {
		card_t * item = card_array_get(&p->hand, i);
		printf("%s ", item->name);
	}
	printf("\n");
}
void print_in_play(player_t * p) {
	unsigned int i;
	printf("[play]\t");
for (i = 0; i < p->in_play.size; i++) {
		card_t * item = card_array_get(&p->in_play, i);
		printf("%s ", item->name);
	}
	printf("\n");
}
void print_discards(player_t * p) {
	unsigned int i;
	printf("[disc]\t");
for (i = 0; i < p->discards.size; i++) {
		card_t * item = card_array_get(&p->discards, i);
		printf("%s ", item->name);
	}
	printf("\n");
}
void print_all(player_t * p) {
	printf("--------player %s--------\n", p->name);
	print_deck(p);
	print_hand(p);
	print_in_play(p);
	print_discards(p);
	printf("\n");
}
void print_every() {
	int i;
	for (i = 0; i < PLAYER_NUM; i++)
		print_all(player + i);
}
void flush_every() {
	int i;
	for (i = 0; i < 24; i++)
		printf("\033[A\033[2K");
}
void wait() {
	fseek(stdin, 0, SEEK_END);
	while(getchar() != '\n');
	printf("\033[A\033[2K");
}

int main(void) {
	
	srand(time(NULL));
	
	if (start_game() == -1)
		return EXIT_FAILURE;
	
	/*	game loop; checks winning conditions before the start of turn  */
	while((empty_pile_num < 3 || (PLAYER_NUM > 4 && empty_pile_num < 4)) && (province.supply > 0) && (!CP_ACTIVE || colony.supply > 0)) {
		
		/*	action phase  */
		while (action_num > 0 && count(&current_player->hand, action) > 0) {
			//	need to decide on the chosen UI before I put anything in these three while loops
		}
		
		/*	treasure phase  */
		while (count(&current_player->hand, action) > 0) {
			
		}
		
		/*	buy phase  */
		while (buy_num > 0) {
			
		}
		
		/*	cleanup phase  */
		card_t * last = card_array_remove(&current_player->hand, current_player->hand.size - 1);	//	eventually the choice for the top of the discard pile will go to the player (once the UI is decided)
		while (current_player->hand.size > 0)
			card_array_transfer(&current_player->hand, 0, &current_player->discards, 0);
		while (current_player->in_play.size > 0)
			card_array_transfer(&current_player->in_play, 0, &current_player->discards, 0);
		card_array_append(&current_player->discards, last);
		
		/*	set up next turn  */
		if (++current_player_num == PLAYER_NUM)
			current_player_num = 0;
		current_player = player + current_player_num;
		action_num = 1;
		buy_num = 1;
		tp_pool = 0;
	}
	
	//determine who won and print the results
	
	wait();
	return EXIT_SUCCESS;
}