# Makefile for the map and visibility module for CS50 nuggets game
# Team 11, Winter 2025
# Mar 8, 2025

OBJS = map.o visibility.o
LIB  = mapVisibility.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc

.PHONY: all clean

all: $(LIB)

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

map.o: map.c map.h
	$(CC) $(CFLAGS) -c map.c

visibility.o: visibility.c visibility.h map.h
	$(CC) $(CFLAGS) -c visibility.c

clean:
	rm -f *~ *.o
	rm -f $(LIB)