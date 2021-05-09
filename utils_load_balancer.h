// Copyright 2020 - 2021 - 311CA - Mihai Daniel Soare
#ifndef UTILS_LOAD_BALANCER_H_
#define UTILS_LOAD_BALANCER_H_

#define FIFTH_PWR 100000

unsigned int hash_function_servers(void *a);

unsigned int hash_function_key(void *a);

int get_server_id(int server_id);

#endif  // UTILS_LOAD_BALANCER_H_
