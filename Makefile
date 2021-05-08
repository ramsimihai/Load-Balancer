CC=gcc
CFLAGS=-std=c99 -Wall -Wextra
LOAD=load_balancer
SERVER=server
HTABLE=Hashtable
LIST=LinkedList
CDLL=circular_doubly_linked_list

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(HTABLE).o $(LIST).o $(CDLL).o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(CDLL).o: $(CDLL).c $(CDLL).h
	$(CC) $(CFLAGS) $^ -c

$(LIST).o: $(LIST).c $(LIST).h
	$(CC) $(CFLAGS) $^ -c

$(HTABLE).o: $(HTABLE).c $(HTABLE).h
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

clean:
	rm -f *.o tema2 *.h.gch