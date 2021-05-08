// Copyright 2020 - 2021 - 311CA - Mihai Daniel Soare
#ifndef CIRCULAR_DOUBLY_LINKED_LIST_H_
#define CIRCULAR_DOUBLY_LINKED_LIST_H_

typedef struct dll_node_t dll_node_t;
struct dll_node_t
{
	void *data;
	dll_node_t *prev, *next;
};

typedef struct doubly_linked_list_t doubly_linked_list_t;
struct doubly_linked_list_t
{
	dll_node_t* head;
	unsigned int data_size;
	unsigned int size;
};

// create a new node with a given data and its size
dll_node_t
*create_node(const void *new_data, unsigned int data_size);

// create a list of a specified data_size
doubly_linked_list_t*
dll_create(unsigned int data_size);

// get the node from a given position
dll_node_t*
dll_get_nth_node(doubly_linked_list_t* list, unsigned int n);

// add a node on the nth position of the list given as a parameter
void
dll_add_nth_node(doubly_linked_list_t* list, unsigned int n, const void* data);

// delete a node from the nth position of the list given as a parameter
dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, unsigned int n);

// free the memory allocated from a list
void
dll_free(doubly_linked_list_t** pp_list);

// print a list that contains int datas
void
dll_print_int_list(doubly_linked_list_t* list);

// move a node in the first position
void
dll_move_first(dll_node_t **head, dll_node_t *new_node);

#endif  // CIRCULAR_DOUBLY_LINKED_LIST_H_
