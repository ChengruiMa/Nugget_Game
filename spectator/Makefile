# Makefile for spectator module
# Team 11, Winter 2025
# Mar 12, 2025

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
OBJS = spectator.o

SUPPORT_DIR = ../support

.PHONY: all clean

all: $(OBJS)

spectator.o: spectator.c spectator.h $(SUPPORT_DIR)/message.h
	$(CC) $(CFLAGS) -I$(SUPPORT_DIR) -c spectator.c

clean:
	rm -f *.o *~