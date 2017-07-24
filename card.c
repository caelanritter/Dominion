
#include "card.h"

int card_array_init(card_array_t * arr, size_t cap, order_e ord) {
	arr->size = 0;
	arr->capacity = cap;
	if(ord != loose && ord != strict && ord != strictest)
		return -1;
	arr->order = ord;
	arr->data = calloc(cap, sizeof(card_t *));
	if(arr->data == NULL)
		return -1;
	else
		return 0;
}
int card_array_free(card_array_t * arr) {
	free(arr->data);
	arr->data = NULL;
	arr->size = arr->capacity = 0;
	return 0;
}
card_t * card_array_get(card_array_t * arr, size_t idx) {
	if(idx >= arr->size)
		return NULL;
	return arr->data[idx];
}
int card_array_set(card_array_t * arr, size_t idx, card_t * item) {
	if(idx >= arr->size)
		return -1;
	arr->data[idx] = item;
	return 0;
}
int card_array_append(card_array_t * arr, card_t * item) {
	if(arr->size >= arr->capacity)
	{
		size_t new_size = sizeof(card_t *) * arr->capacity * 2;
		card_t ** new_data = realloc(arr->data, new_size);
		if(new_data == NULL)
			return -1;
		else {
			arr->capacity *= 2;
			arr->data = new_data;
		}
	}
	arr->data[arr->size++] = item;
	return 0;
}
int card_array_add(card_array_t * arr, size_t idx, card_t * item) {
	if(idx > arr->size)
		return -1;
	if (arr->order == loose)
		return card_array_append(arr, item);
	else {
		if(arr->size >= arr->capacity)
		{
			size_t new_size = sizeof(card_t *) * arr->capacity * 2;
			card_t ** new_data = realloc(arr->data, new_size);
			if(new_data == NULL)
				return -1;
			else {
				arr->capacity *= 2;
				arr->data = new_data;
			}
		}
		unsigned int i;
		for (i = arr->size; i > idx; i--)
			arr->data[i] = arr->data[i - 1];
		arr->data[idx] = item;
		arr->size++;
		return 0;
	}
}
card_t * card_array_remove(card_array_t * arr, size_t idx) {
	if(idx >= arr->size)
		return NULL;
	card_t * item = arr->data[idx];
	if (arr->order == loose) {
		arr->data[idx] = arr->data[arr->size - 1];
		arr->size -= 1;
	} else if (arr->order == strict) {
		unsigned int i;
		for (i = idx; i < arr->size - 1; i++)
			arr->data[i] = arr->data[i + 1];
		arr->size -= 1;
	} else { /*  do not remove if order == strictest  */ }
	return item;
}
int card_array_transfer(card_array_t * org_arr, size_t org_idx, card_array_t * dest_arr, size_t dest_idx) {
	return card_array_add(dest_arr, dest_idx, card_array_remove(org_arr, org_idx));
}
size_t card_array_find(card_array_t * arr, card_t * c) {
	size_t idx;
	for (idx = 0; idx < arr->size; idx++) {
		if (c == card_array_get(arr, idx))
			break;
	}
	if (idx == arr->size)
		return -1;
	else
		return idx;
}