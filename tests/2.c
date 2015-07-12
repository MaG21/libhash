#ifndef lint
static const char rcsid[] =
	"$Id: 2.c,v 1.2 2002/01/29 16:22:11 andrew Exp $";
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

#define REPS	250000

int main(int argc, char *argv[]) {
	hash h;
	int i,
		*p,
		*q,
		values[REPS];
	struct hash_iterator it;

	assert(hash_initialise(&h, 10000U, hash_hash_int, hash_compare_int, hash_copy_int, free, NULL));

	printf("adding\n");

	/* add */
	for (i = 0; i < REPS; ++i) {
		values[i] = i;
		assert(hash_insert(&h, &i, &values[i]));
	}
	/* check for corruption */
	for (i = 0; i < REPS; ++i) {
		assert(values[i] == i);
	}
	/* retrieve */
	for (i = 0; i < REPS; ++i) {
		assert(hash_retrieve(&h, &i, (void **)&p));
		assert(*p == i);
	}

	printf("removing\n");

	/* remove */
	for (i = 0; i < REPS; ++i) {
		assert(hash_delete(&h, &i));
	}
	/* check they are really gone */
	for (i = 0; i < REPS; ++i) {
		assert(! hash_retrieve(&h, &i, (void **)&p));
	}
	/* list all for good measure */
	assert(hash_iterator_initialise(&h, &it));
	assert(! hash_fetch_next(&h, &it, (void **)&p, (void **)&q));
/*	while (hash_fetch_next(&h, &it, (void **)&p, (void **)&q)) {
		printf("%d: %d\n", *p, *q);
	}
*/
	assert(hash_iterator_deinitialise(&h, &it));

	printf("adding\n");

	/* add again (no extra memory should be requested) */
	for (i = 0; i < REPS; ++i) {
		values[i] = i;
		assert(hash_insert(&h, &i, &values[i]));
	}

	printf("killing\n");

	/* free all */
	hash_deinitialise(&h);

	return 0;
}
