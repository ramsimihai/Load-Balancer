// Copyright 2020 - 2021 - 311CA - Mihai Daniel Soare
#ifndef SERVER_H_
#define SERVER_H_

#include "./hashtable.h"

#define HMAX_BUCKETS 93
#define KEY_LENGTH 128
#define VALUE_LENGTH 65536

typedef struct server_memory_t server_memory_t;

struct server_memory_t {
	hashtable_t *data;
};

// init_server_memory() - Allocs a new server_memory_t object.
// Contains a hashtable where the data will be stored.
// A single one is gonna be used a server and two replicas of its own.
server_memory_t* init_server_memory();

// free_server_memory - Frees a server_memory_t object.
// @arg1: Server which is going to be freed.
void free_server_memory(server_memory_t* server);

// server_store() - Stores a key-value pair to the server.
// @arg1: Server which performs the task.
// @arg2: Key represented as a string.
// @arg3: Value represented as a string.
void server_store(server_memory_t* server, char* key, char* value);

// server_remove() - Removes a key-pair value from the server.
// @arg1: Server which performs the task.
// @arg2: Key represented as a string.
void server_remove(server_memory_t* server, char* key);


// server_retrieve() - Gets the value associated with the key.
// @arg1: Server which performs the task.
// @arg2: Key represented as a string.
//
// Return: String value associated with the key
//         or NULL (in case the key does not exist).
char* server_retrieve(server_memory_t* server, char* key);

#endif  // SERVER_H_
