#include <stdlib.h>
#include <stdio.h>

struct dynarr_impl {
    size_t len;
    size_t elem_size;
    size_t allocsize;
};

void dynarr_init_impl(struct dynarr_impl *impl, size_t elem_size, void** data)
{
    impl->len = 0;
    impl->elem_size = elem_size;
    impl->allocsize = 1;
	*data = malloc(impl->allocsize * elem_size);
}

void dynarr_grow_impl(struct dynarr_impl *impl, void** data, int num)
{
	impl->len += num;
	while (impl->len > impl->allocsize) {
		impl->allocsize *= 2;
	}
	*data = realloc(*data, impl->allocsize * impl->elem_size);
}

void dynarr_insert_impl(struct dynarr_impl *impl, void** data, int ind, int num)
{
	if(ind < 0 || ind > impl->len) {
		printf("Can't insert, index (%d) out of bounds [0, %d]!\n", ind, impl->len);
		exit(1);
	}
	dynarr_grow_impl(impl, data, num);
}

void dynarr_pop_impl(struct dynarr_impl *impl, int num)
{
	if(impl->len < num) {
		printf("Can't pop %d elements from array of size %d", num, impl->len);
		exit(1);
	}
	impl->len -= num;
}

void dynarr_remove_impl(struct dynarr_impl *impl, int ind) {
	if(impl->len == 0) {
		printf("Can't remove, length is zero!");
		exit(1);
	}
	if(ind < 0 || ind >= impl->len) {
		printf("Can't remove, index (%d) out of bounds [0, %d)!\n", ind, impl->len);
		exit(1);
	}
	impl->len--;
}

void dynarr_remove_range_impl(struct dynarr_impl *impl, int start, int num) {
	if(start < 0 || start >= impl->len) {
		printf("Can't remove, start index (%d) out of bounds [0, %d)!\n", start, impl->len);
		exit(1);
	}
	if (num > impl->len - start) {
		printf("Can't remove %d elements from array of size %d from start index %d",
			num, impl->len, start);
		exit(1);
	}
	impl->len -= num;
}

#define DYNARR_TEMPLATE(t) struct { struct dynarr_impl impl; t *data; }

#define dynarr_init(a) \
	dynarr_init_impl(&a.impl, sizeof(*a.data), (void**)&a.data); \

#define dynarr_size(a) a.impl.len

#define dynarr_print(a, format) \
	for (int i = 0; i < a.impl.len; i++) { \
		printf(format, a.data[i]); \
	} printf("\n");

#define dynarr_append(a, elem) \
	dynarr_grow_impl(&a.impl, (void**)&a.data, 1); \
	a.data[a.impl.len-1] = elem;

#define dynarr_append_arr(a, arr, num) \
	int prevsize = a.impl.len; \
	dynarr_grow_impl(&a.impl, (void**)&a.data, num); \
	for (int i = 0; i < num; i++) { \
		a.data[i+prevsize] = arr[i]; \
	}

#define dynarr_insert(a, ind, elem) \
	dynarr_insert_impl(&a.impl, (void**)&a.data, ind, 1); \
	for (int i = a.impl.len - 1; i > ind; i--) { \
		a.data[i] = a.data[i - 1]; \
	} \
	a.data[ind] = elem;

#define dynarr_insert_arr(a, ind, arr, num) \
	dynarr_insert_impl(&a.impl, (void**)&a.data, ind, num); \
	for (int i = a.impl.len - 1; i >= ind+num; i--) { \
		a.data[i] = a.data[i - num]; \
	} \
	for (int i = 0; i < num; i++) { \
		a.data[i+ind] = arr[i]; \
	}

#define dynarr_remove(a, ind) \
	dynarr_remove_impl(&a.impl, ind); \
	for (int i = ind; i < a.impl.len; i++) { \
		a.data[i] = a.data[i + 1]; \
	}

#define dynarr_remove_range(a, start, num) \
	dynarr_remove_range_impl(&a.impl, start, num); \
	for (int i = start; i < a.impl.len; i++) { \
		a.data[i] = a.data[i + num]; \
	}

#define dynarr_pop(a) \
	dynarr_pop_impl(&a.impl, 1);

#define dynarr_pop_n(a, num) \
	dynarr_pop_impl(&a.impl, num);

#define dynarr_sort(a, compare) \
	qsort(a.data, a.impl.len, compare)

#define dynarr_free(a) \
	a.impl.len = 0; \
	a.impl.allocsize = 0; \
	a.impl.elem_size = 0; \
	free(a.data)
