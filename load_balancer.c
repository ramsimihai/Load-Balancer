// Copyright 2020 - 2021 - 311CA - Mihai Daniel Soare
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./load_balancer.h"
#include "./utils.h"

load_balancer_t* init_load_balancer() {
	load_balancer_t *load_balancer;

	// allocs the load_balancer
	load_balancer = malloc(sizeof(*load_balancer));
	DIE(NULL == load_balancer, "malloc failed");

	// creates and allocs the hash_ring which is a circular doubly
	// linked list of servers
	load_balancer->hash_ring = dll_create(sizeof(server_t ));

	return load_balancer;
}

void loader_store(load_balancer_t* main, char* key, char* value, int *server_id)
{
	dll_node_t *current = main->hash_ring->head;

	// gets the hash of the object which is going to be added
	uint hash_object = hash_function_key(key);

	// checks if object is going to be stored on the first server
	// or on the last server
	if (hash_object < ((server_t *)current->data)->hash) {
		// stores the (key, value) pair on the server
		server_store(((server_t *)current->data)->server_memory, key, value);

		// gets the server id of the server
		*server_id = get_server_id(((server_t *)current->data)->id);
		return;
	} else if (hash_object > ((server_t *)current->prev->data)->hash) {
		// stores the (key, value) pair on the server
		server_store(((server_t *)current->data)->server_memory, key, value);

		// gets the server id of the server
		*server_id = get_server_id(((server_t *)current->data)->id);
		return;
	}

	// iterates through the list of servers and stores the (key, value) pair on the
	// server which has the hash bigger than the (key, value) pair
	for (uint i = 0; i < main->hash_ring->size; i++) {
		if (hash_object > ((server_t *)current->data)->hash &&
			hash_object < ((server_t *)current->next->data)->hash) {
			// stores the (key, value) pair on the server
			server_store(((server_t *)current->next->data)->server_memory, key, value);

			// gets the server id of the server
			*server_id = get_server_id(((server_t *)current->next->data)->id);
			break;
		}
		current = current->next;
	}
}

char* loader_retrieve(load_balancer_t* main, char* key, int* server_id)
{
	dll_node_t *current = main->hash_ring->head;

	// gets the hash of the object which is going to be looked for
	uint hash_object = hash_function_key(key);

	// checks if the object is stored on the first server or on the last server
	// and retrieve the (key, value) pair
	if (hash_object < ((server_t *)current->data)->hash) {
		*server_id = get_server_id(((server_t *)current->data)->id);

		return server_retrieve(((server_t *)current->data)->server_memory,
								   key);
	} else if (hash_object > ((server_t *)current->prev->data)->hash) {
		*server_id = get_server_id(((server_t *)current->data)->id);

		return server_retrieve(((server_t *)current->data)->server_memory,
								   key);
	}

	// iterates through the list of servers and gets the server which stores
	// the object wanted
	for (uint i = 0; i < main->hash_ring->size; i++) {
		if (hash_object > ((server_t *)current->data)->hash &&
			hash_object < ((server_t *)current->next->data)->hash) {
			// gets the server id and return the value associated to the key
			*server_id = get_server_id(((server_t *)current->next->data)->id);

			return server_retrieve(((server_t *)current->next->data)->server_memory,
								   key);
		}

		current = current->next;
	}

	return NULL;
}

// rebalance the objects of the right neighbour of the added server so the ones
// which has to be on the new added server are moved
void rebalance_hash_ring(doubly_linked_list_t *hash_ring, server_t *server_prev,
						 server_t *server_src, server_t *server_dest)
{
	// checks if the servers exist
	if (!server_src || !server_dest || !server_prev)
		return;

	// gets the first and last server's hash
	uint hash_first_server = ((server_t *)hash_ring->head->next->data)->hash;
	uint hash_last_server = ((server_t *)hash_ring->head->prev->data)->hash;

	// checks if the server_dest is a replica of the original server
	if (server_src->id % FIFTH_PWR == server_dest->id % FIFTH_PWR)
		return;

	// iterates through the array of lists of buckets of the source
	for (uint i = 0; i < server_src->server_memory->data->hmax; i++) {
		ll_node_t *node = server_src->server_memory->data->buckets[i]->head;
		ll_node_t *current;
		int size = server_src->server_memory->data->buckets[i]->size;

		// iterates through every list of buckets
		for (int j = 0; j < size; j++) {
			current = node;
			node = node->next;

			uint hash_obj = hash_function_key(((info_t *)current->data)->key);

			// checks if the object from the server source has the hash lower
			// than the server destination and it is bigger than the
			// previous server of the destination
			if (server_dest->hash > hash_obj && server_prev->hash < hash_obj) {
				// stores the (key, value) pair on the server destination
				ht_put(server_dest->server_memory->data,
					  ((info_t *)current->data)->key, KEY_LENGTH,
					  ((info_t *)current->data)->value, VALUE_LENGTH);

				// removes the (key, value) pair from the source server
				ht_remove_entry(server_src->server_memory->data,
				    		   ((info_t *)current->data)->key);

			// checks if the hash of the first server is lower than the server
			// destination and then checks if the hash object is lower than the
			// server destination or bigger than the last server
			} else if (server_dest->hash < hash_first_server) {
				if (hash_obj > hash_last_server || hash_obj < server_dest->hash) {
					// stores the (key, value) pair on the server destination
					ht_put(server_dest->server_memory->data,
						((info_t *)current->data)->key, KEY_LENGTH,
						((info_t *)current->data) ->value, VALUE_LENGTH);

					// removes the (key, value) pair from the source server
					ht_remove_entry(server_src->server_memory->data,
								   ((info_t *)current->data)->key);
				}
			}
		}
	}
}

// add one server in the hash ring with a specified server id and a specified
// server memory which is used by two replicas of the original
void add_one_server(doubly_linked_list_t *hash_ring,
					int server_id, server_memory_t *new_server_memory)
{
	// initializes a temporary server
	server_t new_server;
	new_server.id = server_id;
	new_server.hash = hash_function_servers(&server_id);
	new_server.server_memory = new_server_memory;

	// add on the first position of the hash ring if there is no server on it
	if (!hash_ring->size) {
		dll_add_nth_node(hash_ring, 0, &new_server);
	} else {
		dll_node_t *current = hash_ring->head;

		// iterates through the list of servers
		for (uint i = 0; i < hash_ring->size; i++) {
			// gets the position of where the new server should be added
			// and add it
			if (new_server.hash >= ((server_t *)current->data)->hash &&
				new_server.hash <= ((server_t *)current->next->data)->hash) {
				dll_add_nth_node(hash_ring, i + 1, &new_server);

				break;
			} else if (new_server.hash >= ((server_t *)current->data)->hash &&
					 i == hash_ring->size - 1) {
				dll_add_nth_node(hash_ring, hash_ring->size, &new_server);

				break;
			} else if (new_server.hash <= ((server_t *)current->data)->hash) {
				dll_add_nth_node(hash_ring, 0, &new_server);
				break;
			}

			current = current->next;
		}
	}

	dll_node_t *current = hash_ring->head;

	// iterates through the hash ring and rebalance the items
	// on the new added server
	for (uint i = 0; i < hash_ring->size; i++) {
		if (((server_t *)current->data)->id == server_id) {
			rebalance_hash_ring(hash_ring, (server_t *)current->prev->data,
					(server_t *)current->next->data, (server_t *)current->data);

			break;
		}

		current = current->next;
	}
}

// add one server and two replicas of its own
void loader_add_server(load_balancer_t* main, int server_id)
{
	// initializes the server memory for the three replicas
	server_memory_t *new_server_memory = init_server_memory();

	// adds the three replicas in the has ring
	for (int i = 0; i <= MAX_REPLIQUE; i++)
		add_one_server(main->hash_ring, i * FIFTH_PWR + server_id,
					   new_server_memory);
}

// check if you really want to delete a server if there is only one server
// and two replicas of it in the hash_ring
void check_delete_one_server(doubly_linked_list_t *hash_ring)
{
	if (hash_ring->size == 3) {
		printf("The data from the server will be erased if you remove the server \n");
		printf("Are you sure?\n");

		char answer[5];
		fscanf(stdin, "%s", answer);

		if (strcmp(answer, "NO") == 0)
			return;
	}
}

// restores every object from the server memory hashtable
// into the load balancer and accordingly into new servers
void move_keys_from_servers(load_balancer_t *main, hashtable_t *data)
{
	// iterates through the array of lists of buckets
	for (uint i = 0; i < data->hmax; i++) {
		ll_node_t *current = data->buckets[i]->head;

		// iterates through every lists of buckets
		uint size = data->buckets[i]->size;
		for (uint j = 0; j < size; j++) {
			// stores every (key, value) pair into the load balancers
			int index_server = 0;
			loader_store(main, ((info_t *)current->data)->key,
						((info_t *)current->data)->value, &index_server);

			current = current->next;
		}
	}
}

void loader_remove_server(load_balancer_t* main, int server_id)
{
	check_delete_one_server(main->hash_ring);

	dll_node_t *node = main->hash_ring->head, *current;
	hashtable_t *tmp_server;
	uint size = main->hash_ring->size;
	int first_found = 0;

	// iterates through the hash ring
	for (uint i = 0; i < size; i++) {
		current = node;
		node = node->next;

		// checks if the server is the wanted server or its replica
		if (((server_t *)current->data)->id % FIFTH_PWR == server_id) {
			first_found++;

			// checks if a server or a replica has been found so it could
			// remove the server memory of the server and get a reference
			// to the hashtable
			if (first_found == 1) {
				tmp_server = ((server_t *)current->data)->server_memory->data;
				free(((server_t *)current->data)->server_memory);
			}

			// checks if the server is by any chance the head of the list
			// so the switches between the head of the list have been made
			if (current == main->hash_ring->head)
				main->hash_ring->head = current->next;

			// switch the links so the server is removed
			current->prev->next = current->next;
			current->next->prev = current->prev;

			// free the server and its data
			free(current->data);
			free(current);
			current = NULL;

			main->hash_ring->size--;
		}
	}

	move_keys_from_servers(main, tmp_server);

	// free the temporary server used to maintain the hashtable
	ht_free(tmp_server);
}

// free the load balancer and its data
void free_load_balancer(load_balancer_t* main)
{
	dll_node_t *current = main->hash_ring->head;

	// iterates through every server of the hash_ring
	for (uint i = 0; i < main->hash_ring->size; i++) {
		// checks if the server is not a replica so it frees its server memory
		// and the data which is the hashtable
		if (((server_t *)current->data)->id % FIFTH_PWR ==
			((server_t *)current->data)->id)
			free_server_memory(((server_t *)current->data)->server_memory);

		current = current->next;
	}

	// frees every server and the load balancer
	dll_free(&main->hash_ring);
	free(main);
}
