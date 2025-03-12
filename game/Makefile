# Makefile for game module
# Team 11, Winter 2025
# Mar 12, 2025

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
OBJS = game.o
LIB = game.a

# Module's Directories
GRID_DIR = ../grid
PLAYER_DIR = ../player
SPECTATOR_DIR = ../spectator
SUPPORT_DIR = ../support

.PHONY: all clean

all: $(LIB)

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

game.o: game.c game.h $(GRID_DIR)/grid.h $(SPECTATOR_DIR)/spectator.h $(SUPPORT_DIR)/message.h $(PLAYER_DIR)/player.h
	$(CC) $(CFLAGS) -I$(GRID_DIR) -I$(SPECTATOR_DIR) -I$(SUPPORT_DIR) -I$(PLAYER_DIR) -c game.c

clean:
	rm -f *.o *~
	rm - f$(LIB)