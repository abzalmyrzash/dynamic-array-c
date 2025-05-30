#include "dynarr.h"
void print_int(void* ptr) {
	printf("%d ", *(int*)ptr);
}

int main()
{
	DynArr dynarr;
	DynArr_init(&dynarr, sizeof(int), 1, DynArrIntFunc);
	int a = 5;
	DynArr_append(&dynarr, &a);
	int b = 6;
	DynArr_append(&dynarr, &b);
	int arr[] = {-1, 1, 2, 3, 4};
	DynArr_insert_arr(&dynarr, 0, arr, 5);
	int c = 0;
	DynArr_insert(&dynarr, 1, &c);
	DynArr_remove(&dynarr, 0);
	DynArr_remove_range(&dynarr, 4, 2);
	print_int(DynArr_pop(&dynarr));
	printf("\n");
	DynArr_pop_n(&dynarr, 2);
	DynArr_iterate(&dynarr, print_int);
	printf("\n");
	return 0;
}
