#include "dynarr.h"

int main()
{
	DynArr dynarr;
	DynArr_init(&dynarr, sizeof(int), 1, DynArrIntFunc);
	DynArr_print(&dynarr);
	int a = 5;
	DynArr_append(&dynarr, &a);
	DynArr_print(&dynarr);
	*(int*)DynArr_append_empty(&dynarr) = 6;
	DynArr_print(&dynarr);
	int arr[] = {-1, 1, 2, 3, 4};
	DynArr_insert_arr(&dynarr, 0, arr, 5);
	DynArr_print(&dynarr);
	int c = 0;
	DynArr_insert(&dynarr, 1, &c);
	DynArr_print(&dynarr);
	int arr2[] = {7, 8};
	DynArr_append_arr(&dynarr, arr, 2);
	DynArr_print(&dynarr);
	DynArr* copy = DynArr_copy(&dynarr);
	DynArr_print(copy);
	DynArr_remove(&dynarr, 0);
	DynArr_print(&dynarr);
	DynArr_remove_range(&dynarr, 4, 2);
	DynArr_print(&dynarr);
	DynArr_pop(&dynarr);
	DynArr_print(&dynarr);
	DynArr_pop_n(&dynarr, 2);
	DynArr_print(&dynarr);
	DynArr_clear(&dynarr);
	DynArr_print(&dynarr);
	DynArr_print(copy);
	return 0;
}
