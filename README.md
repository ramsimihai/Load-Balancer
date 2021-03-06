// Copyright 2020 - 2021 311CA Mihai Daniel Soare

WARNING: i hate to use uppercase letters

PS: this README is a presentation of the program and its going to be added
on github and in CV

Data Structures

Homework #2 Load Balancer

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

!) this is going to be an implementation of a Load Balancer in C using
Consistent Hashing on a hashring which is going to be a circular doubly
linked list;

!) The Load Balancer is a struct defined by a cdll hashring which contains
the servers;
   The Server has an ID, a Hash and a server_memory which is defined as
   a Hashtable;

*) the client of the program can do the following commands:
- add a product (value) associated with an ID (key);
- retrieve the product in function of the ID (key);

*) the developer can use the following commands:
- add a server;
- remove a server;
- store a (key, value) pair;
- retrieve the value associated to the key;

*) there are some following operations which can be done on a server memory:
- store the data received into the server memory;
- remove an entry from the server memory;
- retrieve an entry from the server memory;
 
**) in the file `linked_list` file are operations on linked list that
are going to be needed in the use of implementation of a hashtable;

**) in the file `hashtable` file are operations on a hashtable that
are going to be needed for data of a server memory;

**) in the file `circular_doubly_linked_list` file are operations on cdll that
are going to be needed in the use of the program;

**) in the `utils` file is defined a macro with which the program could verify
if the objects allocated were allocated well;

**) in the `utils_load_balancer` are the functions to hash a server ID, a key of
an object and the function to get a server ID even if it a replica or not;

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

!)
	Consistent Hashing is a method of storing a (key, value) pair on a server
so that they are well distributed and a server hasn't too many objects. Every
server has two replica of its own (has the same server memory) and they are
stored on a hashring. The hashring is an imaginary circle where the hashes of
the servers are sorted in increasing order. The items are distributed 
in function of their hash of the key. The items get on the server with bigger
hash than theirs or on the first server from the hashring if their hash is 
bigger than any of them;

!) 
	The Load Balancer starts by allocating itself memory for containing
the hashring which is defined by a circular doubly linked list of servers.

!) The memory is allocated well and the implementation is easy to understand.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

I)
	- to add a new server, you start by allocating a new server memory which
	is going to be the database for the server and its two replicas;
	- add the server and the replicas by using the following order:
	- create a temporary server with the specified ID, hash and the database;
	- add it in the list of servers by checking where it should be added:
		~ it can be added on the first position, on the last position or
		between two different servers;
	- rebalance the items from the first server which has the hash bigger
	than the last added server;
		~ the items with hash lower than the added server are going to be
		redistributed on it.
		~ if the added server is in the first position, check if there are
		items with has bigger than the last server from the list, because
		they are meant to be on the added server.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

II)
	- to add a new (key, value) pair, it checks in function of the hash of the
key where it should be added:
		~ on the first server, on the last server or between two servers;
	- use the function server_store to add the (key, value) pair on the
	database of that specified server;
	- get the id of the server where it has been added;

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

III)
	- to retrieve a (key, valu) pair, it checks in function of the hash of the
	key where it should be:
		~ on the first server, on the last server or between two server;
	- get the id of the main server even if it is a replica or not of it;
	- return the value stored of the associated key;

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

IV)
	- to remove a server, first verify if there is only one server and its
	replicas so that the data wont be erased and couldn't be retrieved;
	- iterates through the lists of server and remove the server and its
	replicas by firstly removing the database and memorise the data into
	a temporary server memory database and simply switching the links between
	the servers of the hashring;
	- after the removal of the server and the replicas, the (key, value) pairs
	from the database are going to be redistributed in the Load Balancer

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

V)
	- the Load Balancer is freed by iterating through every server of the hash
	ring and freeing the server memory of the original server, then freeing
	the list of server and the load balancer.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
