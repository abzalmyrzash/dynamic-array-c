#include "dynarrv1.h"

int main()
{
	DYNARR_TEMPLATE(int) intArray;
	dynarr_init(intArray);
	printf("%d\n", intArray.data);
	dynarr_append(intArray, 10);
	dynarr_append(intArray, 20);
	dynarr_append(intArray, 30);
	dynarr_insert(intArray, 1, 1);
	printf("%d\n", intArray.impl.len-1);
	// dynarr_remove(intArray, intArray.impl.len-1); // bad
	dynarr_pop_n(intArray, 2);
	dynarr_print(intArray, "%d ");
	return 0;
}
