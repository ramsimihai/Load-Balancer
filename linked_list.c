// Copyright 2020 - 2021 - 311CA - Mihai Daniel Soare
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./linked_list.h"
#include "./utils.h"

typedef unsigned int uint;

int min_int(int a, int b)
{
		if (a > b)
				return b;

		return a;
}

// create a node with a given data and it's size
static ll_node_t *create_node(const void *new_data, uint data_size)
{
	ll_node_t *node;

	// allocs the node and its data and verify if it succeeded
	node = malloc(sizeof(*node));
	DIE(NULL == node, "malloc failed");

	node->data = malloc(data_size);
	DIE(NULL == node->data, "malloc failed");

	// copies the data you want to use in the node
	memcpy(node->data, new_data, data_size);

	return node;
}

linked_list_t*
ll_create(uint data_size)
{
	linked_list_t *list;

	// allocs the list and verify if it succeeded
	list = malloc (sizeof(*list));
	DIE(NULL == list, "malloc failed");

	// initalizes the list
	list->head = NULL;
	list->data_size = data_size;
	list->size = 0;

	return list;
}

// get the node from a given position
static ll_node_t
*get_nth_node(uint n, linked_list_t *list)
{
	ll_node_t *node;

	// checks if the list exists or its size is zero
	if (!list)
		return NULL;

	if (!list->size) {
		fprintf(stdout, "Can't get the nth node\n");
		return NULL;
	}

	// initializes a node with the head
	node = list->head;

	// iterates through the list
	for (uint i = 0; i < n; i++)
		node = node->next;

	return node;
}

void
ll_add_nth_node(linked_list_t* list, uint n, const void* new_data)
{
	ll_node_t *new_node, *prev_node;

	// checks if the list was allocated
	if (!list) {
		fprintf(stderr, "Can't add the node\n");
		return;
	}

	// checks if the position is out of the list and get the position
	// where to add the node
	if (n == 0) {
		n = min_int(n, list->size);
	} else if (n >= list-> size) {
		n = min_int(n, list->size);
	}

	// create a new node
	new_node = create_node(new_data, list->data_size);

	// adds a node on a given position
	if (!n || !list->size) {
		// make the new node the new head and the first node in the list
		new_node->next = list->head;
		list->head = new_node;
	} else {
		// get the previous node of the index where to add the new node
		prev_node = get_nth_node(n - 1, list);
		if (!prev_node) {
			fprintf(stderr, "Couldn't get the node\n");
			return;
		}

		// switches the links between the new node and the prev_node
		new_node->next = prev_node->next;
		prev_node->next = new_node;
	}

	// increments the size of the list
	list->size++;
}

ll_node_t*
ll_remove_nth_node(linked_list_t* list, uint n)
{
	ll_node_t *prev_node, *removed;

	// checks if the list exists or its size is zero
	if (!list || !list->size) {
		fprintf(stderr, "Can't remove the node\n");
		return NULL;
	}

	// checks if the given position is bigger than the list's size
	n = min_int(n, list->size - 1);


	if (!n) {
		// switches the links of the prev and next node of removed
		removed = list->head;
		list->head = list->head->next;
	} else {
		// gets the removed node and checks if it exists
		prev_node = get_nth_node(n - 1, list);

		removed = prev_node->next;
		prev_node->next = prev_node->next->next;
	}

	list->size--;

	return removed;
}

/* get the size of a list */
int
ll_get_size(ll_node_t* head)
{
	int i;

	// iterates through the list
	for (i = 0; head; i++)
		head = head->next;

	return i;
}

void
ll_free(linked_list_t** pp_list)
{
	ll_node_t *node;

	// checks if the list is not allocated
	if (!pp_list || !*pp_list)
		return;

	// iterates through the list and remove every node, then freeing its data
	while ((*pp_list)->size) {
		node = ll_remove_nth_node(*pp_list, 0);

		free(node->data);
		free(node);
	}

	// frees the allocated list
	free(*pp_list);
	*pp_list = NULL;
}
