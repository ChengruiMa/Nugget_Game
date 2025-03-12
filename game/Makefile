# Makefile for game module
# Team 11, Winter 2025
# Mar 12, 2025

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
OBJS = game.o gamemodel.o

# Module's Directories
GRID_DIR = ../grid
PLAYER_DIR = ../player
SPECTATOR_DIR = ../spectator
SUPPORT_DIR = ../support

.PHONY: all clean

all: $(OBJS)

game.o: game.c game.h $(GRID_DIR)/grid.h $(SPECTATOR_DIR)/spectator.h $(SUPPORT_DIR)/message.h $(PLAYER_DIR)/player.h
	$(CC) $(CFLAGS) -I$(GRID_DIR) -I$(SPECTATOR_DIR) -I$(SUPPORT_DIR) -I$(PLAYER_DIR) -c game.c

gamemodel.o: gamemodel.c gamemodel.h game.h $(GRID_DIR)/grid.h $(SPECTATOR_DIR)/spectator.h $(SUPPORT_DIR)/message.h $(PLAYER_DIR)/player.h
	$(CC) $(CFLAGS) -I$(GRID_DIR) -I$(SPECTATOR_DIR) -I$(SUPPORT_DIR) -I$(PLAYER_DIR) -c gamemodel.c

clean:
	rm -f *.o *~