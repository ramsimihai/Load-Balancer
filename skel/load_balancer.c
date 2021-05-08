/* Copyright 2021 <> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"
#include "utils.h"

unsigned int hash_function_servers(void *a) {
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int hash_function_key(void *a) {
	unsigned char *puchar_a = (unsigned char *) a;
	unsigned int hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c;

	return hash;
}

void print_every_server(load_balancer_t *main)
{
	dll_node_t *current = main->hash_ring->head;

	printf("sizeof hashring: %u\n", main->hash_ring->size);

	printf("id si hash - server: ");
	for (int i = 0; i < main->hash_ring->size; i++) {
		printf("%d %u\n", ((server_t*)current->data)->id, ((server_t *)current->data)->hash);
		current = current->next;
	}

}

load_balancer_t* init_load_balancer() {
	load_balancer_t *load_balancer;
	load_balancer = malloc(sizeof(*load_balancer));
	DIE(NULL == load_balancer, "malloc failed");

	load_balancer->hash_ring = dll_create(sizeof(server_t ));

	return load_balancer;
}

int get_server_id(int server_id)
{
	return server_id % 100000;
}

void loader_store(load_balancer_t* main, char* key, char* value, int *server_id)
{
	dll_node_t *current = main->hash_ring->head;
	uint hash_object = hash_function_key(key);
	// print_every_server(main);

	// printf("hash-ul obiectului este: %u\n", hash_object);
	// printf("hash-ul serverului este: %u\n", ((server_t *)current->data)->hash);
	// printf("hash-ul ultimului serverului este: %u\n", ((server_t *)current->prev->data)->hash);

	if (hash_object < ((server_t *)current->data)->hash) {
		server_store(((server_t *)current->data)->server_memory, key, value);
	//	printf("hello");
		*server_id = get_server_id(((server_t *)current->data)->id);
		return;
	} else if (hash_object > ((server_t *)current->prev->data)->hash) {
	//	printf("buna siua");
		server_store(((server_t *)current->data)->server_memory, key, value);
		*server_id = get_server_id(((server_t *)current->data)->id);
		return;
	}

	for (int i = 0; i < main->hash_ring->size; i++) {
		// printf("hash-ul obiectului este: %u\n", hash_object);
		// printf("hash-ul serverului este: %u\n", ((server_t *)current->data)->hash);
		if (hash_object > ((server_t *)current->data)->hash &&
			hash_object < ((server_t *)current->next->data)->hash) {
			server_store(((server_t *)current->next->data)->server_memory, key, value);
			*server_id = get_server_id(((server_t *)current->next->data)->id);
			break;
		} 

		current = current->next;
	}
}

char* loader_retrieve(load_balancer_t* main, char* key, int* server_id)
{
	dll_node_t *current = main->hash_ring->head;
	uint hash_object = hash_function_key(key);

	//print_every_server(main);

	if (hash_object < ((server_t *)current->data)->hash) {
		//printf("salutare");
		*server_id = get_server_id(((server_t *)current->data)->id);
		return server_retrieve(((server_t *)current->data)->server_memory,
								   key);
	} else if (hash_object > ((server_t *)current->prev->data)->hash) {
		//printf("buna siua");
		*server_id = get_server_id(((server_t *)current->data)->id);
		return server_retrieve(((server_t *)current->data)->server_memory,
								   key);
	}

	for (int i = 0; i < main->hash_ring->size; i++) {
		// printf("hash-ul obiectului este: %u\n", hash_object);
		// printf("hash-ul serverului este: %u\n", ((server_t *)current->data)->hash);
		if (hash_object > ((server_t *)current->data)->hash &&
			hash_object < ((server_t *)current->next->data)->hash) {
			*server_id = get_server_id(((server_t *)current->next->data)->id);
			return server_retrieve(((server_t *)current->next->data)->server_memory,
								   key);
		}
		current = current->next;
	}

	return NULL;
}

server_t *create_server(int server_id)
{
	server_t *new_server = malloc(sizeof(*new_server));
	DIE(NULL == new_server, "malloc failed");

	new_server->id = server_id;
	new_server->hash = hash_function_servers(&server_id);
	new_server->server_memory = NULL;

	return new_server;
}

void rebalance_hash_ring(server_t *server_src, server_t *server_dest)
{
	if (!server_src || !server_dest)
		return;

	printf("se face rebalasare\n");
	for (int i = 0; i < server_src->server_memory->data->hmax; i++) {
		ll_node_t *node = server_src->server_memory->data->buckets[i]->head;
		ll_node_t *current;

		int size = server_src->server_memory->data->buckets[i]->size;

		for (int j = 0; j < size; j++) {
			current = node;
			node = node->next;

			if (server_dest->hash >= hash_function_key(((info_t *)current->data)->key)) {
				ht_put(server_dest->server_memory->data, 
					  ((info_t *)current->data)->key, KEY_LENGTH,
					  ((info_t *)current->data)->value, VALUE_LENGTH);

				ht_remove_entry(server_src->server_memory->data, ((info_t *)current->data)->key);
			}
		}
	}
}

void add_one_server(doubly_linked_list_t *hash_ring, int server_id, server_memory_t *new_server_memory)
{
	server_t new_server;
	new_server.id = server_id;
	new_server.hash = hash_function_servers(&server_id);
	new_server.server_memory = new_server_memory;

	if (!hash_ring->size) {
		dll_add_nth_node(hash_ring, 0, &new_server);
	} else {
		dll_node_t *current = hash_ring->head;

		for (uint i = 0; i < hash_ring->size; i++) {
			if (new_server.hash > ((server_t *)current->data)->hash &&
				new_server.hash < ((server_t *)current->next->data)->hash) {
				dll_add_nth_node(hash_ring, i + 1, &new_server);

				break;
			} else if (new_server.hash > ((server_t *)current->data)->hash &&
					 i == hash_ring->size - 1) {
				dll_add_nth_node(hash_ring, hash_ring->size, &new_server);

				break;
			} else if (new_server.hash < ((server_t *)current->data)->hash) {
				dll_add_nth_node(hash_ring, 0, &new_server);
				break;
			}

			current = current->next;
		}
	}

	 dll_node_t *current = hash_ring->head;

	for (int i = 0; i < hash_ring->size; i++) {
		if (((server_t *)current->data)->id == server_id) {
			rebalance_hash_ring((server_t *)current->next->data, (server_t *)current->data);
			break;
		}

		current = current->next;
	}
}

void loader_add_server(load_balancer_t* main, int server_id)
{
	server_memory_t *new_server_memory = init_server_memory();

	for (int i = 0; i <= MAX_REPLIQUE; i++)
		add_one_server(main->hash_ring, i * FIFTH_PWR + server_id,
					   new_server_memory);
}

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

int key_in_server(uint hash_prev, uint hash_src, uint hash_key)
{
	if (hash_prev < hash_key && hash_key < hash_src)
		return 1;

	return 0;
}

void move_keys_from_servers(load_balancer_t *main, hashtable_t *data)
{
	for (int i = 0; i < data->hmax; i++) {
		ll_node_t *current = data->buckets[i]->head;

		for (int j = 0; j < data->buckets[i]->size; i++) {
			int index_server = 0;
			loader_store(main, ((info_t *)current->data)->key,
						((info_t *)current->data)->value, &index_server);

			current = current->next;
		}
	}
}

void move_objects_from_server(load_balancer_t *main, int server_id)
{
	dll_node_t *node = main->hash_ring->head, *current;
	hashtable_t *tmp_server;
	uint size = main->hash_ring->size;
	int first_found = 0;

	for (int i = 0; i < size; i++) {
		current = node;
		node = node->next;
		
		if (((server_t *)current->data)->id % FIFTH_PWR == server_id) {
			first_found++;
			if (first_found == 1) {
				tmp_server = ((server_t *)current->data)->server_memory->data;
				free(((server_t *)current->data)->server_memory);

			}
			// dll_remove_nth_node(main->hash_ring, i - first_found);
			
			if (current == main->hash_ring->head)
				main->hash_ring->head = current->next;

			current->prev->next = current->next;
			current->next->prev = current->prev;

			free(current->data);
			free(current);
			current = NULL;

			main->hash_ring->size--;
		}
		
	}

	//printf("first_found: %d\n", first_found);
	move_keys_from_servers(main, tmp_server);

	ht_free(tmp_server);
}

void loader_remove_server(load_balancer_t* main, int server_id)
{
	move_objects_from_server(main, server_id);
}

void free_load_balancer(load_balancer_t* main)
{
	dll_node_t *current = main->hash_ring->head;
	
	for (int i = 0; i < main->hash_ring->size; i++) {
		if (((server_t *)current->data)->id % FIFTH_PWR == ((server_t *)current->data)->id) {
			ht_free(((server_t *)current->data)->server_memory->data);
			free(((server_t *)current->data)->server_memory);
		}

		current = current->next;
	}

	dll_free(&main->hash_ring);
	free(main);
}
