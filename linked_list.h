// Copyright 2020 - 2021 - 311CA - Mihai Daniel Soare
#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

typedef struct ll_node_t ll_node_t;
struct ll_node_t {
    void* data;
    ll_node_t* next;
};

typedef struct linked_list_t linked_list_t;
struct linked_list_t {
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
};

// return the minimum of two int values
int min_int(int a, int b);

// create a list of a specified data_size
linked_list_t*
ll_create(unsigned int data_size);

// add a node on the nth position of the list given as a parameter
void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* data);

// delete a node from the nth position of the list given as a parameter
ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n);

// get the size of a list
int
ll_get_size(ll_node_t* head);

// free the memory allocated from a list
void
ll_free(linked_list_t** pp_list);

#endif  // LINKED_LIST_H_
