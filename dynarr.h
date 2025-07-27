#pragma once
#include <stddef.h>

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

void DynArr_init(DynArr *a, size_t elem_size, size_t alloc_len, DynArrFunc func);

DynArr* DynArr_new(size_t elem_size, size_t alloc_len, DynArrFunc func);

bool DynArr_check_index(DynArr *a, size_t index);

bool DynArr_check_range(DynArr *a, size_t index, size_t n);

void* DynArr_assign(DynArr* a, size_t index, void* data);

void* DynArr_assign_arr(DynArr* a, size_t index, void* arr, size_t n);

void DynArr_iterate(DynArr *a, void (*f)(void*));

void DynArr_iterate_range(DynArr *a, size_t index, size_t n, void (*f)(void*));

void DynArr_print(DynArr* a);

void DynArr_free(DynArr* a);

void DynArr_free_data(DynArr* a);

void* DynArr_append(DynArr *a, void* elem);

void* DynArr_append_empty(DynArr* a);

void* DynArr_append_arr(DynArr *a, void* arr, size_t n);

void* DynArr_append_arr_empty(DynArr* a, size_t n);

void* DynArr_insert(DynArr *a, size_t ind, void* elem);

void* DynArr_insert_empty(DynArr* a, size_t ind);

void* DynArr_insert_arr(DynArr *a, size_t ind, void* arr, size_t n);

void* DynArr_insert_arr_empty(DynArr* a, size_t ind, size_t n);

void* DynArr_remove(DynArr *a, size_t ind);

void* DynArr_remove_range(DynArr *a, size_t ind, size_t n);

void DynArr_pop(DynArr *a);

void DynArr_pop_n(DynArr *a, size_t n);

void DynArr_clear(DynArr *a);

size_t DynArr_index(DynArr *a, void* ptr);

void DynArr_delete(DynArr *a, void* ptr);

DynArr* DynArr_copy(DynArr *a);

// DynArr functions for some types

const DynArrFunc DynArrFuncNULL = { NULL, NULL, NULL };

void DynArrInt_print(void* data, size_t len);

const DynArrFunc DynArrIntFunc = { DynArrInt_print, NULL, NULL };

void DynArrString_print(void* data, size_t len);

void* DynArrString_copy(void* ptr);

void DynArrString_free(void* ptr);

const DynArrFunc DynArrStringFunc = { DynArrString_print, DynArrString_copy, DynArrString_free };

