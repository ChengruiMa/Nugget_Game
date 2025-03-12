# Makefile for the grid module and its testing for CS50 nuggets game
# Team 11, Winter 2025
# Mar 11, 2025

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
OBJS = grid.o testing.o
LIBS = -lm ../game/game.a

.PHONY: all clean

all: testing

testing: $(OBJS)
	$(CC) $(CFLAGS) -o testing $(OBJS) $(LIBS)

grid.o: grid.c grid.h ../game/game.h
	$(CC) $(CFLAGS) -c grid.c

testing.o: testing.c grid.h
	$(CC) $(CFLAGS) -c testing.c

clean:
	rm -f testing
	rm -f *~ *.o core