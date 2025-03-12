# Makefile for the player module for CS50 nuggets game
# Mar 11, 2025

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
OBJS = player.o
LIB = player.a
LIBS =

GRID_DIR = ../grid
SUPPORT_DIR = ../support

.PHONY: all clean

all: $(LIB)

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

player.o: player.c player.h $(GRID_DIR)/grid.h $(SUPPORT_DIR)/message.h
	$(CC) $(CFLAGS) -I$(GRID_DIR) -I$(SUPPORT_DIR) -c player.c

clean:
	rm -f *.o *~
	rm -f $(LIB)