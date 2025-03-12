# Makefile for spectator module
# Team 11, Winter 2025
# Mar 12, 2025

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
OBJS = spectator.o
LIB = spectator.a

SUPPORT_DIR = ../support

.PHONY: all clean

all: $(LIB)

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

spectator.o: spectator.c spectator.h $(SUPPORT_DIR)/message.h
	$(CC) $(CFLAGS) -I$(SUPPORT_DIR) -c spectator.c

clean:
	rm -f *.o *~
	rm -f $(LIB)