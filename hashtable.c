// Copyright 2020 - 2021 - 311CA - Mihai Daniel Soare
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./utils.h"
#include "./hashtable.h"

typedef unsigned int uint;

#define RESIZE_CONST 0.75

int
compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

unsigned int
hash_function_int(void *a)
{
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int
hash_function_string(void *a)
{
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned int hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c;

	return hash;
}

hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	// allocs the hashtable
	hashtable_t *ht = malloc(sizeof(*ht));
	DIE(ht == NULL, "malloc failed");

	// allocs the lists of the buckets
	ht->buckets = malloc(hmax * sizeof(*ht->buckets));
	DIE(ht->buckets == NULL, "malloc failed");

	// creates every buckets' list
	for (uint i = 0; i < hmax; i++)
		ht->buckets[i] = ll_create(sizeof(info_t));

	// initializes the number of maximum buckets, the hash function
	// and the compare function
	ht->hmax = hmax;

	ht->hash_function = hash_function;
	ht->compare_function = compare_function;

	return ht;
}

static ll_node_t *find_key(linked_list_t *bucket, void *key,
	int (*compare_function)(void*, void*), unsigned int *pos)
{
	// check if the bucket is allocated
	if (!bucket)
		return NULL;

	ll_node_t *current = bucket->head;

	// iterates through the bucket and checks if the key exists in the
	// bucket by using comparing function and return the node where is it found
	for (uint i = 0; i < bucket->size; ++i) {
		if (compare_function(key, ((info_t *)current->data)->key) == 0) {
			*pos = i;
			return current;
		}

		current = current->next;
	}

	return NULL;
}

void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	info_t data;

	// gets the index where to put the key
	uint idx = ht->hash_function(key) % ht->hmax;
	linked_list_t *bucket = ht->buckets[idx];
	uint pos;

	// find the node where it has to be put the new key
	ll_node_t *node = find_key(bucket, key, ht->compare_function, &pos);

	if (!node) {
		// if the node doesnt exist, it gets allocated
		// and deep copy the new (key, value) pair into it
		data.key = malloc(key_size);
		DIE(NULL == data.key, "malloc failed");
		memcpy(data.key, key, key_size);

		data.value = malloc(value_size);
		DIE(NULL == data.value, "malloc failed");
		memcpy(data.value, value, value_size);

		// adds it in the bucket
		ll_add_nth_node(bucket, 0, &data);
	} else {
		// deep copy the value over the old value;
		memcpy(((info_t *)node->data)->value, value, value_size);
	}

	ht->size++;
}

void *
ht_get(hashtable_t *ht, void *key)
{
	// gets the index of the bucket where the key should be
	uint idx = ht->hash_function(key) % ht->hmax;
	linked_list_t *bucket = ht->buckets[idx];
	uint pos;

	// gets the node of the given key
	ll_node_t *node = find_key(bucket, key, ht->compare_function, &pos);

	// returns it if the key is stored in the hashtable
	if (node)
		return ((info_t *)node->data)->value;

	return NULL;
}

int
ht_has_key(hashtable_t *ht, void *key)
{
	// gets the index of the bucket where the key should be
	uint idx = ht->hash_function(key) % ht->hmax;
	linked_list_t *bucket = ht->buckets[idx];
	uint pos;

	// gets the node of the given key
	ll_node_t *node = find_key(bucket, key, ht->compare_function, &pos);

	if (node)
		return 1;

	return 0;
}

void
ht_remove_entry(hashtable_t *ht, void *key)
{
	// checks if the hashtable is allocated
	if (!ht)
		return;

	// gets the index of the bucket where the key should be
	uint idx = ht->hash_function(key) % ht->hmax;
	linked_list_t *bucket = ht->buckets[idx];
	uint pos;

	// gets the node of the given key
	ll_node_t *node = find_key(bucket, key, ht->compare_function, &pos);

	// if the key associated exists in hashtable removes the node
	if (node) {
		ll_node_t *removed = ll_remove_nth_node(bucket, pos);

		// frees the (key, value) pair and the node
		free(((info_t *)removed->data)->value);
		free(((info_t *)removed->data)->key);

		free(removed->data);
		free(removed);

		// decrements the entries size of the hashtable
		ht->size--;
	}
}

void
ht_free(hashtable_t *ht)
{
	ll_node_t *current;

	// iterates through every list of buckets
	for (uint i = 0; i < ht->hmax; i++) {
		// checks if the bucket of the hashtable is allocated
		if (ht->buckets[i]->head != NULL) {
			current = ht->buckets[i]->head;

			// iterates through a bucket and free the (key, value) pairs
			while (current != NULL) {
				free(((info_t *)current->data)->value);
				free(((info_t *)current->data)->key);

				current = current->next;
			}
		}

		// frees every list of buckets
		ll_free(&ht->buckets[i]);
	}

	// frees the array of buckets and the hashtable
	free(ht->buckets);
	free(ht);
}

unsigned int
ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int
ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}
