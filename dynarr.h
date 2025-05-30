#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	int len;
	int alloc_size;
	int elem_size;
	void* data;
} DynArr;

void DynArr_init(DynArr *a, int elem_size, int alloc_size) {
	a->elem_size = elem_size;
	a->alloc_size = alloc_size;
	a->len = 0;
	a->data = malloc(a->alloc_size * elem_size);
}

void DynArr_grow(DynArr *a, int n) {
	a->len += n;
	while (a->len > a->alloc_size) {
		a->alloc_size *= 2;
	}
	a->data = realloc(a->data, a->alloc_size * a->elem_size);
}

void DynArr_shrink(DynArr *a, int n) {
	if (a->len < n) {
		printf("Can't remove %d elements from array of size %d!\n", n, a->len);
		exit(1);
	}
	a->len -= n;
}

void* DynArr_at(DynArr* a, int index) {
	return a->data + index * a->elem_size;
}

void DynArr_iterate(DynArr *a, void (*f)(void*)) {
	for(int i = 0; i < a->len; i++) {
		(*f)(DynArr_at(a, i));
	}
}
		
void DynArr_append(DynArr *a, void* elem) {
	int prev_len = a->len;
	DynArr_grow(a, 1);
	memcpy(DynArr_at(a, prev_len), elem, a->elem_size);
}

void DynArr_append_arr(DynArr *a, void* arr, int n) {
	int prev_len = a->len;
	DynArr_grow(a, n);
	memcpy(DynArr_at(a, prev_len), arr, n * a->elem_size);
}

void DynArr_insert(DynArr *a, int ind, void* elem) {
	if(ind < 0 || ind > a->len) {
		printf("Can't insert, index (%d) out of bounds [0, %d]!\n", ind, a->len);
		exit(1);
	}
	int prev_len = a->len;
	DynArr_grow(a, 1);
	int num_to_move = prev_len - ind;
	memmove(DynArr_at(a, ind+1), DynArr_at(a, ind), num_to_move * a->elem_size);
	memcpy(DynArr_at(a, ind), elem, a->elem_size);
}

void DynArr_insert_arr(DynArr *a, int ind, void* arr, int n) {
	if(ind < 0 || ind > a->len) {
		printf("Can't insert, index (%d) out of bounds [0, %d]!\n", ind, a->len);
		exit(1);
	}
	int prev_len = a->len;
	DynArr_grow(a, n);
	int num_to_move = prev_len - ind;
	memmove(DynArr_at(a, ind+n), DynArr_at(a, ind), num_to_move * a->elem_size);
	memcpy(DynArr_at(a, ind), arr, n * a->elem_size);
}

void DynArr_remove(DynArr *a, int ind) {
	if(ind < 0 || ind >= a->len) {
		printf("Can't remove, index (%d) out of bounds [0, %d)!\n", ind, a->len);
		exit(1);
	}
	int prev_len = a->len;
	DynArr_shrink(a, 1);
	int num_to_move = prev_len - ind;
	memmove(DynArr_at(a, ind), DynArr_at(a, ind+1), num_to_move * a->elem_size);
}

void DynArr_remove_range(DynArr *a, int ind, int n) {
	if(ind < 0 || ind >= a->len) {
		printf("Can't remove, index (%d) out of bounds [0, %d)!\n", ind, a->len);
		exit(1);
	}
	if (ind + n > a->len) {
		printf("Can't remove %d elements from array of size %d starting from index %d!\n",
			n, a->len, ind);
		exit(1);
	}
	int prev_len = a->len;
	DynArr_shrink(a, n);
	int num_to_move = prev_len - ind;
	memmove(DynArr_at(a, ind), DynArr_at(a, ind+n), num_to_move * a->elem_size);
}

void* DynArr_pop(DynArr *a) {
	DynArr_shrink(a, 1);
	return DynArr_at(a, a->len);
}

void DynArr_pop_n(DynArr *a, int n) {
	DynArr_shrink(a, n);
}

