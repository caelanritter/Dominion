
#include "library.h"

card_t curse = { "curse", victory, 10, 0, -1, 0, NULL, NULL, NULL, NULL };
card_t estate = { "estate", victory, 8, 2, 1, 0, NULL, NULL, NULL, NULL };
card_t duchy = { "duchy", victory, 8, 5, 3, 0, NULL, NULL, NULL, NULL };
card_t province = { "province", victory, 8, 8, 6, 0, NULL, NULL, NULL, NULL };
card_t colony = { "colony", victory, 8, 11, 10, 0, NULL, NULL, NULL, NULL };
card_t copper = { "copper", treasure, 46, 0, 0, 1, NULL, NULL, NULL, NULL };
card_t silver = { "silver", treasure, 40, 3, 0, 2, NULL, NULL, NULL, NULL };
card_t gold = { "gold", treasure, 30, 6, 0, 3, NULL, NULL, NULL, NULL };
card_t platinum = { "platinum", treasure, 12, 9, 0, 5, NULL, NULL, NULL, NULL };

/*  ------------kingdom cards------------  */

card_t artisan = { "artisan", action, 10, 6, 0, 0, artisan_act, NULL, NULL, NULL };
int artisan_act(va_list val) {
	player_t * p = va_arg(val, player_t *);
	int ret = gain(&p->hand, va_arg(val, card_t *));
	if (ret == -1)
		return -1;
	return place(&p->hand, va_arg(val, card_t *), &p->deck);
}

card_t smithy = { "smithy", action, 10, 3, 0, 0, smithy_act, NULL, NULL, NULL };
int smithy_act(va_list val) {
	player_t * p = va_arg(val, player_t *);
	int i;
	for(i = 0; i < 3; i++)
		draw(p);
	return 0;
}

card_t chapel = { "chapel", action, 10, 2, 0, 0, chapel_act, NULL, NULL, NULL };
int chapel_act(va_list val) {
	player_t * p = va_arg(val, player_t *);
	unsigned int n = va_arg(val, unsigned int);
	if (n > 4)
		return -1;
	unsigned int i;
	int ret = 0;
	for (i = 0; i < n; i++)
		ret -= trash(&p->hand, va_arg(val, card_t *));
	if (ret < 0)
		return -1;
	return 0;
}