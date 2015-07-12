/* $Id: hash.h,v 1.1.1.1 2002/01/13 16:22:30 andrew Exp $ */
#ifndef HASH_H
#define HASH_H

#include <sys/types.h>

struct hash_entry {
	void *key;
	void *value;

	struct hash_entry *next;
};

struct hash {
	/* array of buckets */
	struct hash_entry **buckets;
	/* number of buckets (length of bucket array) */
	u_int32_t number_of_buckets;

	/* compare 2 keys */
	int (* compare)(void *key1, void *key2);
	/* generate hash */
	u_int32_t (*hash_function)(void *key, u_int32_t number_of_buckets);
	/* free keys */
	void (*free_key)(void *key);
	/* free values */
	void (*free_value)(void *value);
	/* copy keys */
	int (*duplicate_key)(void **destination, void *source);
};

struct hash_iterator {
	u_int32_t bucket_index;
	struct hash_entry *entry;
};

typedef struct hash hash;

/* convenience functions */
int hash_compare_int(void *key1, void *key2);
int hash_compare_string(void *key1, void *key2);
u_int32_t hash_hash_int(void *key, u_int32_t number_of_buckets);
u_int32_t hash_hash_string(void *key, u_int32_t number_of_buckets);
int hash_copy_int(void **destination, void *source);
int hash_copy_string(void **destination, void *source);

/* public interface */
int hash_initialise(hash *h, u_int32_t number_of_buckets, u_int32_t (*hash_function)(void *key, u_int32_t number_of_buckets), int (*compare_keys)(void *key1, void *key2), int (*duplicate_key)(void **destination, void *source), void (*free_keys)(void *key), void (*free_values)(void *value));
int hash_insert(hash *h, void *key, void *value);
int hash_retrieve(hash *h, void *key, void **value);
int hash_delete(hash *h, void *key);
int hash_iterator_initialise(hash *h, struct hash_iterator *it);
int hash_fetch_next(hash *h, struct hash_iterator *it, void **key, void **value);
int hash_iterator_deinitialise(hash *h, struct hash_iterator *it);
void hash_deinitialise(hash *h);

#endif
