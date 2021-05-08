// Copyright 2020 - 2021 - 311CA - Mihai Daniel Soare
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./circular_doubly_linked_list.h"
#include "./utils.h"

typedef unsigned int uint;

dll_node_t *create_node(const void *new_data, uint data_size)
{
	dll_node_t *node;

	// allocs the node and its data and verify if it happened
	node = malloc(sizeof(*node));
	DIE(NULL == node, "malloc failed");

	node->data = malloc(data_size);
	DIE(NULL == node->data, "malloc failed");

	// copies the data you want to use in the node
	memcpy(node->data, new_data, data_size);

	return node;
}

doubly_linked_list_t*
dll_create(uint data_size)
{
	doubly_linked_list_t *list;

	// allocs the list and verify if it happened
	list = malloc(sizeof(*list));
	DIE(NULL == list, "malloc failed");

	// initializes the list
	list->head = NULL;
	list->data_size = data_size;
	list->size = 0;

	return list;
}

dll_node_t*
dll_get_nth_node(doubly_linked_list_t* list, uint n)
{
	dll_node_t *node;
	uint i;

	// checks if the list exists or its size is zero
	if (!list)
		return NULL;


	if (!list->size) {
		fprintf(stdout, "Cant get the nth node\n");
		return NULL;
	}

	// initializes a node with the head
	node = list->head;

	// calculates the index of the node wanted to be returned
	n = n % list->size;

	// iterates through the list
	for (i = 0; i < n; i++)
		node = node->next;

	return node;
}

void
dll_add_nth_node(doubly_linked_list_t* list, uint n, const void* data)
{
	dll_node_t *new_node, *prev_node, *curr_node;

	// checks if the list was allocated
	if (!list) {
		fprintf(stderr, "Can't add the node\n");
		return;
	}

	// checks if the position is out of the list and add on the last position
	if (n >= list->size)
		n = list->size;

	// create a new node
	new_node = create_node(data, list->data_size);

	// adds node on a given position
	if (!list->size) {
		// make the new node the new head and the first node in the list
		new_node->next = new_node;
		new_node->prev = new_node;
		list->head = new_node;
	} else if (n == 0) {
		// adds a node in the front of the list
		curr_node = list->head;

		// links the new_node to the prev of curr and the current node;
		new_node->next = curr_node;
		new_node->prev = curr_node->prev;

		// switches the links of the prev and curr_node;
		curr_node->prev->next = new_node;
		curr_node->prev = new_node;

		// make the new node the head of the list
		list->head = new_node;
	} else {
		// gets the previous node from a list on the n - 1 position;
		prev_node = dll_get_nth_node(list, n - 1);

		// switches the links between the new node and the prev node
		new_node->next = prev_node->next;
		prev_node->next->prev = new_node;

		prev_node->next = new_node;
		new_node->prev = prev_node;
	}

	// increments the size of the list
	list->size++;
}

dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, uint n)
{
	dll_node_t *removed;

	// checks if the list exists or its size is zero
	if (!list || !list->size) {
		fprintf(stderr, "Can't remove the node\n");
		return NULL;
	}

	// checks if the given position is bigger than the list's size
	if (n >= list->size)
		n = list->size - 1;

	// gets the removed node
	if (!n) {
		removed = list->head;
		list->head = removed->next;

	} else {
		removed = dll_get_nth_node(list, n);
	}

	// switches the links of the prev and next node of removed
	removed->next->prev = removed->prev;
	removed->prev->next = removed->next;

	// decrements the size of the list
	list->size--;

	// checks if the list is empty and initialize the head with NULL
	if (!list->size)
		list->head = NULL;

	return removed;
}

void
dll_free(doubly_linked_list_t** pp_list)
{
	dll_node_t *node;

	// checks if the list is not allocated
	if (!pp_list || !*pp_list)
		return;

	// iterates through the list and remove every node, then freeing its data
	while((*pp_list)->size) {
		node = dll_remove_nth_node(*pp_list, 0);

		free(node->data);
		free(node);
	}

	// frees the allocated list
	free(*pp_list);
	*pp_list = NULL;
}
