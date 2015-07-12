#ifndef lint
static const char rcsid[] =
	"$Id: hash.c,v 1.5 2005/02/06 18:18:39 andrew Exp $";
#endif

#include <sys/types.h>

#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "hash.h"

/* convenience functions */

int hash_compare_string(void *key1, void *key2) {

	return strcmp((char *)key1, (char *)key2);
}

int hash_compare_int(void *key1, void *key2) {

	return (*(int *)key1 - *(int *)key2);
}

u_int32_t hash_hash_string(void *key, u_int32_t number_of_buckets) {
	u_int32_t hash_value;

	hash_value = 0U;

	while (*(char *)key != '\0') {
		hash_value += *(char *)key;
		key = (void *)((char *)key + 1);
	}

	return hash_value % number_of_buckets;
}

u_int32_t hash_hash_int(void *key, u_int32_t number_of_buckets) {

	return *(int *)key % number_of_buckets;
}

int hash_copy_int(void **destination, void *source) {

	if ((*destination = malloc(sizeof(int))) == NULL) {
		return 0;
	}
	(void)memcpy(*destination, source, sizeof(int));
	
	return 1;
}

int hash_copy_string(void **destination, void *source) {

	if ((*destination = strdup((const char *const)source)) == NULL) {
		return 0;
	} else {
		return 1;
	}
}

/* library interface */

int hash_initialise(hash *h, u_int32_t number_of_buckets, u_int32_t (*hash_function)(void *key, u_int32_t number_of_buckets), int (*compare)(void *key1, void *key2), int (*duplicate_key)(void **destination, void *src), void (*free_key)(void *key), void (*free_value)(void *value)) {

	/* allocate space for bucket array */
	if ((h->buckets = (struct hash_entry **)calloc(number_of_buckets, sizeof(struct hash_entry *))) == NULL) {
		return 0;
	}

	/* store data for later use */
	h->number_of_buckets = number_of_buckets;
	if (compare == NULL) {
		h->compare = hash_compare_string;
	} else {
		h->compare = compare;
	}
	if (hash_function == NULL) {
		h->hash_function = hash_hash_string;
	} else {
		h->hash_function = hash_function;
	}
	if (duplicate_key == NULL) {
		h->duplicate_key = hash_copy_string;
	} else {
		h->duplicate_key = duplicate_key;
	}
	h->free_key = free_key;
	h->free_value = free_value;

	return 1;
}

int hash_insert(hash *h, void *key, void *value) {
	int i;
	struct hash_entry *new_entry, *prev_entry, *curr_entry, *condemed_entry;
	u_int32_t bucket_index;

	/* create an entry for our data */
	if ((new_entry = (struct hash_entry *)malloc(sizeof(struct hash_entry))) == NULL) {
		return 0;
	}

	/* fill in the entry */
	if (h->duplicate_key == NULL) {
		new_entry->key = key;
	} else {
		h->duplicate_key(&new_entry->key, key);
	}
	new_entry->value = value;

	/* find where our entry should be */
	bucket_index = h->hash_function(key, h->number_of_buckets);

	/* find where to put our new entry, keeping list sorted and keeping an
	 * eye out for duplicates */
	prev_entry = NULL;
	for (curr_entry = h->buckets[bucket_index]; curr_entry != NULL; prev_entry = curr_entry, curr_entry = curr_entry->next) {
		if ((i = h->compare(key, curr_entry->key)) == 0) {
			/* we have a match so we have to free */
			if (h->free_key != NULL) {
				h->free_key(curr_entry->key);
			}
			if (h->free_value != NULL) {
				h->free_value(curr_entry->value);
			}
			condemed_entry = curr_entry;
			curr_entry = curr_entry->next;
			free(condemed_entry);
			/* now we need to insert */
			break;
		} else if (i < 0) {
			/* this is our spot */
			break;
		}
	}

	/* add entry */
	new_entry->next = curr_entry;
	if (prev_entry != NULL) {
		prev_entry->next = new_entry;
	} else {
		h->buckets[bucket_index] = new_entry;
	}

	return 1;
}

int hash_retrieve(hash *h, void *key, void **value) {
	int i;
	struct hash_entry *curr_entry;
	u_int32_t bucket_index;

	/* find where our entry should be */
	bucket_index = h->hash_function(key, h->number_of_buckets);

	/* search list for entry */
	for (curr_entry = h->buckets[bucket_index]; curr_entry != NULL; curr_entry = curr_entry->next) {
		if ((i = h->compare(key, curr_entry->key)) == 0) {
			/* this is it */
			*value = curr_entry->value;
			return 1;
		} else if (i < 0) {
			/* were it here then we would have found it by now */
			errno = ENOENT;
			return 0;
		}
	}

	/* feel off the end so not found */
	errno = ENOENT;
	return 0;
}

int hash_delete(hash *h, void *key) {
	int i;
	struct hash_entry *curr_entry, *prev_entry;
	u_int32_t bucket_index;

	/* find where our entry should be */
	bucket_index = h->hash_function(key, h->number_of_buckets);

	/* search list for entry */
	prev_entry = NULL;
	for (curr_entry = h->buckets[bucket_index]; curr_entry != NULL; prev_entry = curr_entry, curr_entry = curr_entry->next) {
		if ((i = h->compare(key, curr_entry->key)) == 0) {
			/* this is it - take entry out of list */
			if (prev_entry == NULL) {
				/* removing first item in list */
				h->buckets[bucket_index] = curr_entry->next;
			} else {
				/* removing item from middle or end */
				prev_entry->next = curr_entry->next;
			}
			/* free memory used by entry */
			if (h->free_key != NULL) {
				h->free_key(curr_entry->key);
			}
			if (h->free_value != NULL) {
				h->free_value(curr_entry->value);
			}
			free(curr_entry);
			/* let the user know we got it */
			return 1;
		} else if (i < 0) {
			/* were it here then we would have found it by now */
			errno = ENOENT;
			return 0;
		}
	}

	/* we reached the end of the list without finding it */
	errno = ENOENT;
	return 0;
}

int hash_iterator_initialise(hash *h, struct hash_iterator *it) {

	it->bucket_index = 0;
	it->entry = h->buckets[0];

	return 1;
}

int hash_fetch_next(hash *h, struct hash_iterator *it, void **key, void **value) {

	for (;;) {
		if (it->entry == NULL) {
			/* reached the end of this list so move down the backbone a step */
			++it->bucket_index;
			if (it->bucket_index >= h->number_of_buckets) {
				/* we've fallen off the end so there can be no more data to
				   return. we reset the iterator so it can be used again and
				   return false */
				it->bucket_index = 0;
				it->entry = h->buckets[0];
				errno = ENOENT;
				return 0;
			} else {
				/* point it->entry to the first bucket in this list */
				it->entry = h->buckets[it->bucket_index];
			}
		} else {
			/* we have some data to return */
			*key = it->entry->key;
			*value = it->entry->value;
			/* point to the next item in the list ready for the next call */
			it->entry = it->entry->next;
			/* indicate success */
			return 1;
		}
	}
}

int hash_iterator_deinitialise(hash *h __attribute__((__unused__)), struct hash_iterator *it __attribute__((__unused__))) {

	return 1;
}

void hash_deinitialise(hash *h) {
	struct hash_entry *curr, *condemed;
	u_int32_t bucket_index;

	/* for each list */
	for (bucket_index = 0; bucket_index < h->number_of_buckets; ++bucket_index) {
		/* for each entry in the list */
		for (curr = h->buckets[bucket_index]; curr != NULL; /* done in loop */) {
			condemed = curr;
			curr = curr->next;
			/* free entry */
			if (h->free_key != NULL) {
				h->free_key(condemed->key);
			}
			if (h->free_value != NULL) {
				h->free_value(condemed->value);
			}
			free(condemed);
		}
	}

	/* free backbone */
	free(h->buckets);
}
