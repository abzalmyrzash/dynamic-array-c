#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// a struct that holds pointers to functions that operate on an array's elements
typedef struct {
	void (*print)(void*, int); // ptr to func for printing all elements
	void (*free)(void*); // ptr to func for freeing individual element pointers
} DynArrFunc;

DynArrFunc DynArrFuncNULL = { NULL, NULL };

void DynArrInt_print(void* data, int len) {
	for (int i = 0; i < len; i++) {
		printf("%d ", *((int*)data + i));
	}
	printf("\n");
}

DynArrFunc DynArrIntFunc = { DynArrInt_print, NULL };

void DynArrString_print(void* data, int len) {
	for (int i = 0; i < len; i++) {
		printf("%s\n", *((char**)data + i));
		// printf("%s\n", *((char**)data + len));
	}
}

void DynArrString_free(void* ptr) {
	free(*(char**)ptr);
}

DynArrFunc DynArrStringFunc = { DynArrString_print, DynArrString_free };

typedef struct {
	int len;
	int alloc_size;
	int elem_size;
	void* data;
	DynArrFunc func;
} DynArr;

void DynArr_init(DynArr *a, int elem_size, int alloc_size, DynArrFunc func) {
	a->elem_size = elem_size;
	a->alloc_size = alloc_size;
	a->len = 0;
	a->data = malloc(a->alloc_size * elem_size);
	if (a->data == NULL) {
		perror("Unable to allocate dynamic array data");
		exit(1);
	}
	a->func = func;
}

DynArr* DynArr_new(int elem_size, int alloc_size, DynArrFunc func) {
	DynArr* a = malloc(sizeof(DynArr));
	if (a == NULL) {
		perror("Unable to allocate dynamic array");
		exit(1);
	}
	DynArr_init(a, elem_size, alloc_size, func);
	return a;
}

void* DynArr_at(DynArr* a, int index) {
	return a->data + index * a->elem_size;
}

void* DynArr_assign(DynArr* a, int index, void* data) {
	return memcpy(DynArr_at(a, index), a->data, a->elem_size);
}

void DynArr_iterate(DynArr *a, void (*f)(void*)) {
	for(int i = 0; i < a->len; i++) {
		(*f)(DynArr_at(a, i));
	}
}

void DynArr_print(DynArr* a) {
	if(a->func.print != NULL) {
		a->func.print(a->data, a->len);
	}
}

void DynArr_free(DynArr* a) {
	if(a == NULL) return;
	if(a->func.free != NULL) {
		DynArr_iterate(a, a->func.free);
	}
	free(a);
}

void DynArr_grow(DynArr *a, int n) {
	a->len += n;
	while (a->len > a->alloc_size) {
		a->alloc_size *= 2;
	}
	a->data = realloc(a->data, a->alloc_size * a->elem_size);
	if (a->data == NULL) {
		perror("Unable to reallocate dynamic array data");
		exit(1);
	}
}

void DynArr_shrink(DynArr *a, int n) {
	if (a->len < n) {
		printf("Can't remove %d elements from array of size %d!\n", n, a->len);
		exit(1);
	}
	a->len -= n;
}

void* DynArr_append(DynArr *a, void* elem) {
	int prev_len = a->len;
	DynArr_grow(a, 1);
	return memcpy(DynArr_at(a, prev_len), elem, a->elem_size);
}

void* DynArr_append_arr(DynArr *a, void* arr, int n) {
	int prev_len = a->len;
	DynArr_grow(a, n);
	return memcpy(DynArr_at(a, prev_len), arr, n * a->elem_size);
}

void* DynArr_insert(DynArr *a, int ind, void* elem) {
	if(ind < 0 || ind > a->len) {
		printf("Can't insert, index (%d) out of bounds [0, %d]!\n", ind, a->len);
		exit(1);
	}
	int prev_len = a->len;
	DynArr_grow(a, 1);
	int num_to_move = prev_len - ind;
	memmove(DynArr_at(a, ind+1), DynArr_at(a, ind), num_to_move * a->elem_size);
	return memcpy(DynArr_at(a, ind), elem, a->elem_size);
}

void* DynArr_insert_arr(DynArr *a, int ind, void* arr, int n) {
	if(ind < 0 || ind > a->len) {
		printf("Can't insert, index (%d) out of bounds [0, %d]!\n", ind, a->len);
		exit(1);
	}
	int prev_len = a->len;
	DynArr_grow(a, n);
	int num_to_move = prev_len - ind;
	memmove(DynArr_at(a, ind+n), DynArr_at(a, ind), num_to_move * a->elem_size);
	return memcpy(DynArr_at(a, ind), arr, n * a->elem_size);
}

void* DynArr_remove(DynArr *a, int ind) {
	if(ind < 0 || ind >= a->len) {
		printf("Can't remove, index (%d) out of bounds [0, %d)!\n", ind, a->len);
		exit(1);
	}
	int prev_len = a->len;
	DynArr_shrink(a, 1);
	int num_to_move = prev_len - ind;
	return memmove(DynArr_at(a, ind), DynArr_at(a, ind+1), num_to_move * a->elem_size);
}

void* DynArr_remove_range(DynArr *a, int ind, int n) {
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
	return memmove(DynArr_at(a, ind), DynArr_at(a, ind+n), num_to_move * a->elem_size);
}

void* DynArr_pop(DynArr *a) {
	DynArr_shrink(a, 1);
	return DynArr_at(a, a->len);
}

void* DynArr_pop_n(DynArr *a, int n) {
	DynArr_shrink(a, n);
	return DynArr_at(a, a->len);
}

int DynArr_index(DynArr *a, void* ptr) {
	return (ptr - a->data) / a->elem_size;
}

void DynArr_delete(DynArr *a, void* ptr) {
	int index = DynArr_index(a, ptr);
	DynArr_remove(a, index);
}
