
#ifndef CARD_H
#define CARD_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef struct player_t player_t;

typedef enum { victory = 1, treasure = 2, action = 4 } type_e;

/*	a certain card must only ever be initialized once; you can have more of them by having more pointers to a single card  */
typedef struct card_t {
	char name[24];
	type_e type;
	unsigned int supply;
	unsigned int cost;
	int vp;
	int tp;
	int (*act)(va_list);
	int (*react)(va_list);
	int (*upon_buy)(va_list);
	int (*upon_gain)(va_list);
} card_t;

typedef enum { loose = 0, strict = 1, strictest = 2 } order_e;

typedef struct {
	size_t size;
	size_t capacity;
	order_e order;
	card_t ** data;
} card_array_t;

int card_array_init(card_array_t *, size_t, order_e);
int card_array_free(card_array_t *);
card_t * card_array_get(card_array_t *, size_t);
int card_array_set(card_array_t *, size_t, card_t *);
int card_array_append(card_array_t *, card_t *);
int card_array_add(card_array_t *, size_t, card_t *);
card_t * card_array_remove(card_array_t *, size_t);
int card_array_transfer(card_array_t *, size_t, card_array_t *, size_t);
size_t card_array_find(card_array_t *, card_t *);

#endif	/*  CARD_H  */
