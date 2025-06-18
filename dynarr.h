#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// a struct that holds pointers to functions that operate on an array's elements
typedef struct {
	void (*print)(void*, int); // func for printing elements
	void* (*copy)(void*); // func for deep copying elements
	void (*free)(void*); // func for freeing nested pointers
} DynArrFunc;

DynArrFunc DynArrFuncNULL = { NULL, NULL, NULL };

void DynArrInt_print(void* data, int len) {
	for (int i = 0; i < len; i++) {
		printf("%d ", *((int*)data + i));
	}
	printf("\n");
}

DynArrFunc DynArrIntFunc = { DynArrInt_print, NULL, NULL };

void DynArrString_print(void* data, int len) {
	for (int i = 0; i < len; i++) {
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

DynArrFunc DynArrStringFunc = { DynArrString_print, DynArrString_copy, DynArrString_free };

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

void _DynArr_check_index(DynArr *a, int index) {
	if (index < 0 || index >= a->len) {
		printf("ERROR: index out of bounds!\n");
		exit(1);
	}
}

void _DynArr_check_range(DynArr *a, int index, int n) {
	if (index < 0 || index + n < a->len) {
		printf("ERROR: Invalid range (i=%d, n=%d) for array (%d)!\n",
			index, n, a->len);
		exit(1);
	}
}

void* DynArr_at(DynArr* a, int index) {
	return a->data + index * a->elem_size;
}

void* DynArr_assign(DynArr* a, int index, void* data) {
	_DynArr_check_index(a, index);
	if(a->func.copy != NULL) {
		data = a->func.copy(data);
	}
	void* ptr = memcpy(DynArr_at(a, index), data, a->elem_size);
	if(a->func.copy != NULL) {
		free(data);
	}
	return ptr;
}

void* DynArr_assign_arr(DynArr* a, int index, void* arr, int n) {
	_DynArr_check_range(a, index, n);
	for(int i = 0; i < n; i++) {
		DynArr_assign(a, index+i, arr + i * a->elem_size);
	}
	return DynArr_at(a, index);
}

void DynArr_iterate(DynArr *a, void (*f)(void*)) {
	for(int i = 0; i < a->len; i++) {
		(*f)(DynArr_at(a, i));
	}
}

void DynArr_iterate_range(DynArr *a, int index, int n, void (*f)(void*)) {
	_DynArr_check_range(a, index, n);
	for(int i = 0; i < n; i++) {
		(*f)(DynArr_at(a, index+i));
	}
}

void DynArr_print(DynArr* a) {
	if(a->func.print != NULL) {
		a->func.print(a->data, a->len);
	}
}

void DynArr_free(DynArr* a) {
	if(a->func.free != NULL) {
		DynArr_iterate(a, a->func.free);
	}
	free(a->data);
	free(a);
}

void _DynArr_free_elem(DynArr* a, int index) {
	if (a->func.free == NULL) return;
	a->func.free(DynArr_at(a, index));
}

void _DynArr_free_range(DynArr* a, int index, int n) {
	if (a->func.free == NULL) return;
	DynArr_iterate_range(a, index, n, a->func.free);
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
		printf("ERROR: Can't shrink array (%d) by %d!\n", a->len, n);
		exit(1);
	}
	a->len -= n;
}

void* DynArr_append(DynArr *a, void* elem) {
	int prev_len = a->len;
	DynArr_grow(a, 1);
	return DynArr_assign(a, prev_len, elem);
}

void* DynArr_append_arr(DynArr *a, void* arr, int n) {
	int prev_len = a->len;
	DynArr_grow(a, n);
	return DynArr_assign_arr(a, prev_len, arr, n);
}

void* DynArr_insert(DynArr *a, int ind, void* elem) {
	_DynArr_check_index(a, ind);
	int prev_len = a->len;
	DynArr_grow(a, 1);
	int num_to_move = prev_len - ind;
	memmove(DynArr_at(a, ind+1), DynArr_at(a, ind), num_to_move * a->elem_size);
	return DynArr_assign(a, ind, elem);
}

void* DynArr_insert_arr(DynArr *a, int ind, void* arr, int n) {
	_DynArr_check_range(a, ind, n);
	int prev_len = a->len;
	DynArr_grow(a, n);
	int num_to_move = prev_len - ind;
	memmove(DynArr_at(a, ind+n), DynArr_at(a, ind), num_to_move * a->elem_size);
	return DynArr_assign_arr(a, prev_len, arr, n);
}

void* DynArr_remove(DynArr *a, int ind) {
	_DynArr_check_index(a, ind);
	int prev_len = a->len;
	_DynArr_free_elem(a, ind);
	DynArr_shrink(a, 1);
	int num_to_move = prev_len - ind;
	return memmove(DynArr_at(a, ind), DynArr_at(a, ind+1), num_to_move * a->elem_size);
}

void* DynArr_remove_range(DynArr *a, int ind, int n) {
	_DynArr_check_range(a, ind, n);
	int prev_len = a->len;
	_DynArr_free_range(a, ind, n);
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
	if (index < 0 || index >= a->len) {
		printf("ERROR: failed to delete, ptr outside dynamic array!\n");
		exit(1);
	}
	DynArr_remove(a, index);
}
