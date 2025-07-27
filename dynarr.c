#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>

// a struct that holds pointers to functions that operate on an array's elements
typedef struct {
	void (*print)(void*, size_t); // func for printing elements
	void* (*copy)(void*); // func for deep copying elements
	void (*free)(void*); // func for freeing nested pointers
} DynArrFunc;

typedef struct {
	size_t len;
	size_t alloc_len;
	size_t elem_size;
	void* data;
	DynArrFunc func;
} DynArr;

void DynArr_init(DynArr *a, size_t elem_size, size_t alloc_len, DynArrFunc func) {
	a->elem_size = elem_size;
	if (alloc_len < 1) a->alloc_len = 1;
	else a->alloc_len = alloc_len;
	a->len = 0;
	a->data = malloc(a->alloc_len * elem_size);
	if (a->data == NULL) {
		perror("Unable to allocate dynamic array data");
		exit(1);
	}
	a->func = func;
}

DynArr* DynArr_new(size_t elem_size, size_t alloc_len, DynArrFunc func) {
	DynArr* a = malloc(sizeof(DynArr));
	if (a == NULL) {
		perror("Unable to allocate dynamic array");
		exit(1);
	}
	DynArr_init(a, elem_size, alloc_len, func);
	return a;
}

inline bool DynArr_check_index(DynArr *a, size_t index) {
	return (index >= 0 && index < a->len);
}

inline bool DynArr_check_range(DynArr *a, size_t index, size_t n) {
	return (index >= 0 && index + n <= a->len);
}

inline void* DynArr_at(DynArr* a, size_t index) {
	return a->data + index * a->elem_size;
}

void* DynArr_assign(DynArr* a, size_t index, void* data) {
	assert(DynArr_check_index(a, index));
	if(a->func.copy != NULL) {
		data = a->func.copy(data);
	}
	void* ptr = memcpy(DynArr_at(a, index), data, a->elem_size);
	if(a->func.copy != NULL) {
		free(data);
	}
	return ptr;
}

void* DynArr_assign_arr(DynArr* a, size_t index, void* arr, size_t n) {
	assert(DynArr_check_range(a, index, n));
	for(size_t i = 0; i < n; i++) {
		DynArr_assign(a, index+i, arr + i * a->elem_size);
	}
	return DynArr_at(a, index);
}

void DynArr_iterate(DynArr *a, void (*f)(void*)) {
	for(size_t i = 0; i < a->len; i++) {
		(*f)(DynArr_at(a, i));
	}
}

void DynArr_iterate_range(DynArr *a, size_t index, size_t n, void (*f)(void*)) {
	assert(DynArr_check_range(a, index, n));
	for(size_t i = 0; i < n; i++) {
		(*f)(DynArr_at(a, index+i));
	}
}

void DynArr_print(DynArr* a) {
	if(a->func.print != NULL) {
		a->func.print(a->data, a->len);
	}
}

void DynArr_free_data(DynArr* a) {
	if (a == NULL) return;
	if(a->func.free != NULL) {
		DynArr_iterate(a, a->func.free);
	}
	free(a->data);
}

void DynArr_free(DynArr* a) {
	if (a == NULL) return;
	DynArr_free_data(a);
	free(a);
}

static void DynArr_free_elem(DynArr* a, size_t index) {
	a->func.free(DynArr_at(a, index));
}

static void DynArr_free_range(DynArr* a, size_t index, size_t n) {
	DynArr_iterate_range(a, index, n, a->func.free);
}

static inline void DynArr_grow(DynArr *a, size_t n) {
	a->len += n;
	size_t old_size = a->alloc_len;
	assert(a->alloc_len > 0);
	while (a->len > a->alloc_len) {
		a->alloc_len *= 2;
	}
	if(a->alloc_len != old_size) {
		a->data = realloc(a->data, a->alloc_len * a->elem_size);
		if (a->data == NULL) {
			perror("Unable to reallocate dynamic array data");
			exit(1);
		}
	}
}

static inline void DynArr_shrink(DynArr *a, size_t n) {
	assert(n <= a->len);
	a->len -= n;
}

void* DynArr_append(DynArr *a, void* elem) {
	size_t prev_len = a->len;
	DynArr_grow(a, 1);
	return DynArr_assign(a, prev_len, elem);
}

void* DynArr_append_empty(DynArr* a) {
	DynArr_grow(a, 1);
	return DynArr_at(a, a->len - 1);
}

void* DynArr_append_arr(DynArr *a, void* arr, size_t n) {
	if (n < 0) {
		printf("ERROR in DynArr_append_arr: negative n!\n");
		exit(1);
	}
	size_t prev_len = a->len;
	DynArr_grow(a, n);
	return DynArr_assign_arr(a, prev_len, arr, n);
}

void* DynArr_append_arr_empty(DynArr* a, size_t n) {
	DynArr_grow(a, n);
	return DynArr_at(a, a->len - n);
}

void* DynArr_insert(DynArr *a, size_t ind, void* elem) {
	assert(DynArr_check_index(a, ind));
	size_t prev_len = a->len;
	DynArr_grow(a, 1);
	size_t num_to_move = prev_len - ind;
	memmove(DynArr_at(a, ind+1), DynArr_at(a, ind), num_to_move * a->elem_size);
	return DynArr_assign(a, ind, elem);
}

void* DynArr_insert_empty(DynArr* a, size_t ind) {
	assert(DynArr_check_index(a, ind));
	size_t prev_len = a->len;
	DynArr_grow(a, 1);
	size_t num_to_move = prev_len - ind;
	memmove(DynArr_at(a, ind+1), DynArr_at(a, ind), num_to_move * a->elem_size);
	return DynArr_at(a, ind);
}

void* DynArr_insert_arr(DynArr *a, size_t ind, void* arr, size_t n) {
	assert(DynArr_check_index(a, ind));
	size_t prev_len = a->len;
	DynArr_grow(a, n);
	size_t num_to_move = prev_len - ind;
	memmove(DynArr_at(a, ind+n), DynArr_at(a, ind), num_to_move * a->elem_size);
	return DynArr_assign_arr(a, ind, arr, n);
}

void* DynArr_insert_arr_empty(DynArr* a, size_t ind, size_t n) {
	assert(DynArr_check_index(a, ind));
	size_t prev_len = a->len;
	DynArr_grow(a, n);
	size_t num_to_move = prev_len - ind;
	memmove(DynArr_at(a, ind+n), DynArr_at(a, ind), num_to_move * a->elem_size);
	return DynArr_at(a, ind);
}

void* DynArr_remove(DynArr *a, size_t ind) {
	assert(DynArr_check_index(a, ind));
	size_t prev_len = a->len;
	if(a->func.free != NULL) DynArr_free_elem(a, ind);
	DynArr_shrink(a, 1);
	size_t num_to_move = prev_len - ind;
	return memmove(DynArr_at(a, ind), DynArr_at(a, ind+1), num_to_move * a->elem_size);
}

void* DynArr_remove_range(DynArr *a, size_t ind, size_t n) {
	assert(DynArr_check_range(a, ind, n));
	size_t prev_len = a->len;
	if(a->func.free != NULL) DynArr_free_range(a, ind, n);
	DynArr_shrink(a, n);
	size_t num_to_move = prev_len - ind;
	return memmove(DynArr_at(a, ind), DynArr_at(a, ind+n), num_to_move * a->elem_size);
}

void DynArr_pop(DynArr *a) {
	if(a->func.free != NULL) DynArr_free_elem(a, a->len - 1);
	DynArr_shrink(a, 1);
}

void DynArr_pop_n(DynArr *a, size_t n) {
	if(a->func.free != NULL) DynArr_free_range(a, a->len - n, n);
	DynArr_shrink(a, n);
}

void DynArr_clear(DynArr *a) {
	DynArr_pop_n(a, a->len);
}

size_t DynArr_index(DynArr *a, void* ptr) {
	return (ptr - a->data) / a->elem_size;
}

void DynArr_delete(DynArr *a, void* ptr) {
	size_t index = DynArr_index(a, ptr);
	if (index < 0 || index >= a->len) {
		printf("ERROR: failed to delete, ptr outside dynamic array!\n");
		exit(1);
	}
	DynArr_remove(a, index);
}

DynArr* DynArr_copy(DynArr *a) {
	DynArr* copy = DynArr_new(a->elem_size, a->alloc_len, a->func);
	DynArr_append_arr(copy, a->data, a->len);
	return copy;
}

// DynArr functions for some types

void DynArrInt_print(void* data, size_t len) {
	for (size_t i = 0; i < len; i++) {
		printf("%d ", *((int*)data + i));
	}
	printf("\n");
}

void DynArrString_print(void* data, size_t len) {
	for (size_t i = 0; i < len; i++) {
		printf("%s\n", *((char**)data + i));
	}
}

void* DynArrString_copy(void* ptr) {
	char** strptr = ptr;
	char** copyptr = malloc(sizeof(char*));
	*copyptr = malloc(strlen(*strptr) + 1);
	strcpy(*copyptr, *strptr);
	return copyptr;
}

void DynArrString_free(void* ptr) {
	free(*(char**)ptr);
}

