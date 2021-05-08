/* Copyright 2021 <> */
#ifndef SERVER_H_
#define SERVER_H_

#include "Hashtable.h"

#define HMAX_BUCKETS 89
#define KEY_LENGTH 128
#define VALUE_LENGTH 65536

typedef struct server_memory_t server_memory_t;

struct server_memory_t {
	hashtable_t *data;	
};

server_memory_t* init_server_memory();

void free_server_memory(server_memory_t* server);

/**
 * server_store() - Stores a key-value pair to the server.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 * @arg3: Value represented as a string.
 */
void server_store(server_memory_t* server, char* key, char* value);

/**
 * server_remove() - Removes a key-pair value from the server.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 */
void server_remove(server_memory_t* server, char* key);

/**
 * server_retrieve() - Gets the value associated with the key.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 *
 * Return: String value associated with the key
 *         or NULL (in case the key does not exist).
 */
char* server_retrieve(server_memory_t* server, char* key);

#endif  /* SERVER_H_ */
