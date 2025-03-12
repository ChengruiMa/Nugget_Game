# Makefile for the grid module and its testing for CS50 nuggets game
# Mar 11, 2025

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
OBJS = grid.o
TOBJS = testing.o
LIBS = -lm

.PHONY: all clean

all: grid.a



# grid: $(OBJS)
# 	$(CC) $(CFLAGS) -o grid $(OBJS) $(LIBS)

grid.a: $(OBJS)
	ar cr grid.a $(OBJS)

grid.o: grid.c grid.h 
	$(CC) $(CFLAGS) -c grid.c

testing: $(OBJS) $(TOBJS)
	$(CC) $(CFLAGS) -o testing $(OBJS) $(TOBJS) $(LIBS)

testing.o: testing.c grid.h
	$(CC) $(CFLAGS) -c testing.c

clean:
	rm -f testing
	rm -f *~ *.o core