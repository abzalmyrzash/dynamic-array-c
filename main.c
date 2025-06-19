#include "dynarr.h"
void print_int(void* ptr) {
	printf("%d ", *(int*)ptr);
}

int main()
{
	DynArr dynarr;
	DynArr_init(&dynarr, sizeof(int), 1, DynArrIntFunc);
	DynArr_print(&dynarr);
	int a = 5;
	DynArr_append(&dynarr, &a);
	DynArr_print(&dynarr);
	int b = 6;
	DynArr_append(&dynarr, &b);
	DynArr_print(&dynarr);
	int arr[] = {-1, 1, 2, 3, 4};
	DynArr_insert_arr(&dynarr, 0, arr, 5);
	DynArr_print(&dynarr);
	int c = 0;
	DynArr_insert(&dynarr, 1, &c);
	DynArr_print(&dynarr);
	DynArr_remove(&dynarr, 0);
	DynArr_print(&dynarr);
	DynArr_remove_range(&dynarr, 4, 2);
	DynArr_print(&dynarr);
	print_int(DynArr_pop(&dynarr));
	printf("\n");
	DynArr_pop_n(&dynarr, 2);
	DynArr_print(&dynarr);
	return 0;
}
