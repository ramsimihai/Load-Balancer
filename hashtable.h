// Copyright 2020 - 2021 - 311CA - Mihai Daniel Soare
#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "./linked_list.h"

typedef struct {
	void *key;
	void *value;
} info_t;

typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	// array of buckets
	linked_list_t **buckets;
	// number of total entries in the buckets
	unsigned int size;
	// total number of buckets
	unsigned int hmax;
	// pointer to a hash_function
	unsigned int (*hash_function)(void*);
	// pointer to a compare function
	int (*compare_function)(void*, void*);
};

// create a hashtable with a maximum number of buckets
hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*));

// put a specified (key, value) pair with specified sizes into a hashtable
void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size);

// get the node where the associated value of the given key is found
void *
ht_get(hashtable_t *ht, void *key);

// check if the hashtable has a specified key
int
ht_has_key(hashtable_t *ht, void *key);

// remove a value which is associated with a key in the hashtable
void
ht_remove_entry(hashtable_t *ht, void *key);

// get the size of the hashtable
unsigned int
ht_get_size(hashtable_t *ht);

// get the size of buckets
unsigned int
ht_get_hmax(hashtable_t *ht);

// free the hashtable and its entries
void
ht_free(hashtable_t *ht);

// compare two int values
int
compare_function_ints(void *a, void *b);

// compare two strings
int
compare_function_strings(void *a, void *b);

// creating a hash for an int value with a very stable implementation
// so that two values dont have the same hash
unsigned int
hash_function_int(void *a);

// creating a hash for a string with a very stable implementation
// so that two strings dont have the same hash
unsigned int
hash_function_string(void *a);

#endif  // HASHTABLE_H_
