#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "dlinked-list.h"
#include "card.h"

/* a basic player engine */
char * engine_big_money_smithy(player_t *);
/* waits for human input */
char * engine_human(player_t *);

int main(void) {
  //player_t player[NUM_PLAYERS];
  int i = 0;
  int game_end = 0;
  
  /* setup */
  /* Currently seeds random() with system time (in seconds); this may not be the right solution to random (say, if one simulates many games in parallel) */
  srandom(time(NULL));
  for (i = 0; i < NUM_PLAYERS; i++) {
    int j;
    player_list[i] = (player_t){ "", NULL, NULL, NULL, NULL, 0, 0, 0, END, 0, engine_big_money_smithy };
    if (i == 0) { player_list[0].engine = engine_human; }
    sprintf(player_list[i].name, "Player %c", 'A' + i);
    for (j = 0; j < 7; j++) { gain(player_list + i, &copper); }
    for (j = 0; j < 3; j++) { gain(player_list + i, &estate); }
    // tmp
    /* gain(player_list + i, &chapel); */
    /* gain(player_list + i, &smithy); */
    // end tmp
    shuffle(player_list + i);
    for (j = 0; j < 5; j++) { draw(player_list + i); }
  }
  switch (NUM_PLAYERS) {
  case 2:
    estate.supply = 8;
    duchy.supply = 8;
    province.supply = 8;
    break;
  case 3:
    estate.supply = 12;
    break;
  case 4:
    break;
  default:
    printf("NUM_PLAYERS must be 2, 3, or 4\n");
    return 0;
  }

  /* game loop */
  for (i = 0; 1; i = (i + 1) % NUM_PLAYERS) {
    player_list[i].num_actions = 1;
    player_list[i].num_buys = 1;
    player_list[i].num_coins = 0;
    player_list[i].phase = ACT;

    player_print(player_list + i, player_list);

    /* turn loop */
    while (player_list[i].phase != END) {
      int j, arg_idx = 0, arg_pos = 0;
      //int command_buffer_length 48;
      int arg_buffer_length = 12;
      int arg_num = 4;
      char **arg = calloc(arg_num, sizeof(char *));

      /* player command */
      char *command = player_list[i].engine(player_list + i);
      if (player_list[i].engine != engine_human) { printf("%s\n", command); }
      
      /* parse command into arguments */
      for (j = 0; ; j++) {
	/* handle allocation and reallocation of argument buffers */
	if (arg_pos == 0) {
	  arg[arg_idx] = calloc(arg_buffer_length, sizeof(char));
	}
	else if (arg_pos == arg_buffer_length) {
	  arg_buffer_length *= 2;
	  arg[arg_idx] = reallocarray(arg[arg_idx], arg_buffer_length, sizeof(char));
	}
	/* parse next character */
	if (command[j] != ' ' && command[j] != '\n' && command[j] != '\0') {
	  arg[arg_idx][arg_pos] = command[j];
	  arg_pos++;
	}
	else if ((command[j] == ' ' || command[j] == '\n') && arg_pos != 0) {
	  arg[arg_idx][arg_pos] = '\0';
	  arg_idx++;
	  if (arg_idx == arg_num) {
	    arg_num *= 2;
	    arg = reallocarray(arg, arg_num, sizeof(char *));
	  }
	  arg_pos = 0;
	}
	else if (command[j] == '\0') {
	  if (arg_pos == 0) {
	    free(arg[arg_idx]);
	    arg_num = arg_idx;
	  }
	  else {
	    arg_num = arg_idx + 1;
	  }
	  arg = reallocarray(arg, arg_num + 1, sizeof(char *));
	  arg[arg_num] = NULL; // null-terminates `arg`
	  break;
	}
      }
      
      /* printf("debug\n"); */
      /* for (arg_idx = 0; arg_idx < arg_num; arg_idx++) { */
      /* 	printf(arg[arg_idx]); */
      /* 	printf("\n"); */
      /* } */
      
      /* execute command */
      card_t *card = NULL;
      if (arg_num == 0) { continue; }
      if (!strcmp(arg[0], "pass")) { break; }
      if (!strcmp(arg[0], "look")) {
	player_print(player_list + i, player_list + i);
	continue;
      }
      if (arg_num == 1) { continue; }
      else {
	card = str_to_card(arg[1]);
	if (card == NULL) { continue; }
      }
      if (!strcmp(arg[0], "buy")) {
	if (buy(player_list + i, card)) { continue; }
      }
      else if (!strcmp(arg[0], "play")) {
	if (card->type == VICTORY) { continue; }
	else if (card->type == TREASURE) {
	  if (player_list[i].phase == ACT) { player_list[i].phase = BUY; }
	  if (play_treasure(player_list + i, card)) { continue; }
	}
	else if (card->type == ACTION) {
	  if (player_list[i].phase != ACT || player_list[i].num_actions == 0) { continue; }
	  if (!play(player_list + i, card, arg + 2)) { player_list[i].num_actions--; }
	}
      }
      else if (!strcmp(arg[0], "supply")) {
	printf("%d\n", card->supply);
      }
      else { continue; }

      /* check for game end */
      if (province.supply == 0 || num_empty_piles >= 3) {
	game_end = 1;
	break;
      }
      /* check for phase end */
      if (player_list[i].num_actions == 0 && player_list[i].phase == ACT) { player_list[i].phase = BUY; }
      if (player_list[i].num_buys == 0 && player_list[i].phase == BUY) { player_list[i].phase = END; }
    }
    if (game_end) { break; }
    
    /* cleanup */
    {
      int j;
      printf("cleanup\n");
      move_all(&player_list[i].hand, &player_list[i].discard);
      move_all(&player_list[i].in_play, &player_list[i].discard);
      for (j = 0; j < 5; j++) { draw(player_list + i); }
    }

    /* wait for user, then clear the screen */
    printf("NEXT TURN\n");
    char tmp[10] = "";
    fgets(tmp, 10, stdin);
    printf("\e[1;1H\e[2J");
  }

  /* scoring */
  printf("=== GAME END ===\n");
  printf("last to play was Player %c\n", 'A' + i);
  for (i = 0; i < NUM_PLAYERS; i++) {
    printf("Player %c: %d\n", 'A' + i, player_list[i].vp);
  }

  /* old way of determining the score */
  /* int vp[NUM_PLAYERS]; */
  /* int score_in_list(node_t *current) { */
  /*   int vp = 0; */
  /*   if (!current) { return vp; } */
  /*   while (current->prev) { current = current->prev; } */
  /*   while(current) { */
  /*     vp += current->card->vp; */
  /*     current = current->next; */
  /*   } */
  /*   return vp; */
  /* } */
  /* for (i = 0; i < NUM_PLAYERS; i++) { */
  /*   vp[i] = 0; */
  /*   vp[i] += score_in_list(player_list[i].deck); */
  /*   vp[i] += score_in_list(player_list[i].hand); */
  /*   vp[i] += score_in_list(player_list[i].in_play); */
  /*   vp[i] += score_in_list(player_list[i].discard); */
  /* } */
}

char * engine_human(player_t *player) {
  /* weird stuff happens if one removes `static` and hits enter a bunch of times */
  static char *command;
  size_t command_size;
  /* play all treasures */
  if (command && !strcmp(command, "t\n")) {
    node_t *current = player->hand;
    while (current) {
      if (current->card->type == TREASURE) {
	static char command_[30] = "";
	snprintf(command_, 30, "%s %s", "play", current->card->name);
	return command_;
      }
      current = current->next;
    }
  }
  free(command);
  command = NULL;
  getline(&command, &command_size, stdin);
  return command;
}

char * engine_big_money_smithy(player_t *player) {
  switch (player->phase) {
  case ACT:
    /* first play a smithy if possible */
    if (find(player->hand, &smithy)) {
      return "play smithy";
    }
  case BUY:
    double smithy_ratio, critical_smithy_ratio;
    /* play a treasure if possible */
    node_t *current = player->hand;
    while (current) {
      if (current->card->type == TREASURE) {
	static char command[30] = "";
	snprintf(command, 30, "%s %s", "play", current->card->name);
	return command;
      }
      current = current->next;
    }
    /* determine the ratio of smithies in the entire deck */
    {
      int smithy_count = 0, total = 0;
      if (!strcmp(player->name, "Player A")) { critical_smithy_ratio = 0.1; }
      else { critical_smithy_ratio = 0.2; }
      int count_cards(node_t *current, card_t *card) {
	if (current == NULL) { return 0; }
	while (current->prev != NULL) { current = current->prev; }
	if (card == NULL) {
	  return node_length(current);
	}
	int count = 0;
	while(current != NULL) {
	  count += (current->card == card);
	  current = current->next;
	}
	return count;
      }
      smithy_count += count_cards(player->deck, &smithy);
      smithy_count += count_cards(player->hand, &smithy);
      smithy_count += count_cards(player->in_play, &smithy);
      smithy_count += count_cards(player->discard, &smithy);
      total += count_cards(player->deck, NULL);
      total += count_cards(player->hand, NULL);
      total += count_cards(player->in_play, NULL);
      total += count_cards(player->discard, NULL);
      smithy_ratio = (double)smithy_count / total;
    }
    /* buy phase */
    if (player->num_coins >= 8) { return "buy province"; }
    else if (player->num_coins >= 5 && province.supply == 1) { return "buy duchy"; }
    else if (player->num_coins >= 2 && province.supply == 1) { return "buy estate"; }
    else if (player->num_coins >= 6) { return "buy gold"; }
    else if (player->num_coins >= 4 && smithy_ratio < critical_smithy_ratio) { return "buy smithy"; }
    else if (player->num_coins >= 3) { return "buy silver"; }
  case END:
    break;
  }
  return "pass";
}
