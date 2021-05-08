// Copyright 2020 - 2021 - 311CA - Mihai Daniel Soare
#include <stdlib.h>
#include <string.h>

#include "./server.h"
#include "./utils.h"

server_memory_t* init_server_memory() {
	// allocates the server memory of the new server
	server_memory_t *new_server_memory = malloc(sizeof(*new_server_memory));
	DIE(NULL == new_server_memory, "malloc failed");

	// creates the hashtable of the server_memory
	new_server_memory->data = ht_create(HMAX_BUCKETS, hash_function_string,
										compare_function_strings);

	return new_server_memory;
}

void server_store(server_memory_t* server, char* key, char* value)
{
	// stores the (key, value) pair in the hashtable associated with the server
	ht_put(server->data, key, KEY_LENGTH, value, VALUE_LENGTH);
}

void server_remove(server_memory_t* server, char* key)
{
	// removes the (key, value) pair in the hashtable associated with the server
	ht_remove_entry(server->data, key);
}

char* server_retrieve(server_memory_t* server, char* key)
{
	// gets the value associated with a specified key from the hashtable
	return ht_get(server->data, key);
}

void free_server_memory(server_memory_t* server)
{
	// frees a server hashtable and its memory
	ht_free(server->data);
	free(server);
}
