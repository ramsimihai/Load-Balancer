# Copyright 2020 - 2021 - 311CA - Mihai Daniel Soare
CC=gcc
CFLAGS=-std=c99 -Wall -Wextra
LOAD=load_balancer
SERVER=server
HTABLE=hashtable
LIST=linked_list
CDLL=circular_doubly_linked_list
UTL=utils_load_balancer

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(HTABLE).o $(LIST).o $(CDLL).o $(UTL).o
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

$(UTL).o: $(UTL).c $(UTL).h
	$(CC) $(CFLAGS) $^ -c

pack:
	zip -FSr 311CA_MihaiDaniel_Soare_Tema2.zip README Makefile *.c *.h

clean:
	rm -f *.o tema2 *.h.gch