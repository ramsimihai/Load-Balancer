// Copyright 2020 - 2021 - 311CA - Mihai Daniel Soare
#include <stdio.h>

#include "./utils_load_balancer.h"

// creating a hash for a server with a very stable implementation
// so that two servers dont have the same hash
unsigned int hash_function_servers(void *a)
{
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

// creating a hash for a key with a very stable implementation
// so that two keys dont have the same hash
unsigned int hash_function_key(void *a)
{
	unsigned char *puchar_a = (unsigned char *) a;
	unsigned int hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c;

	return hash;
}

// get the server id even if it is a replica of the main server
int get_server_id(int server_id)
{
	return server_id % FIFTH_PWR;
}
