# Makefile for nuggets client

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
LIBS = -lm -lncurses

SRCS = client.c client_state.c display.c network.c
OBJS = $(SRCS:.c=.o)
HDRS = client_state.h display.h network.h

PROG = client

SUPPORT_DIR = ../support
GRID_DIR = ../grid

SUPPORT_OBJS = $(SUPPORT_DIR)/message.o $(SUPPORT_DIR)/log.o
GRID_OBJS = $(GRID_DIR)/grid.o

.PHONY: all clean

all: $(PROG)

$(PROG): $(OBJS) $(SUPPORT_OBJS) $(GRID_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

client.o: client.c client_state.h display.h network.h $(SUPPORT_DIR)/message.h $(SUPPORT_DIR)/log.h
client_state.o: client_state.c client_state.h $(SUPPORT_DIR)/log.h $(GRID_DIR)/grid.h
display.o: display.c display.h client_state.h $(SUPPORT_DIR)/log.h
network.o: network.c network.h client_state.h display.h $(SUPPORT_DIR)/message.h $(SUPPORT_DIR)/log.h

%.o: %.c
	$(CC) $(CFLAGS) -I$(SUPPORT_DIR) -I$(GRID_DIR) -c $<

clean:
	rm -f $(PROG) *~ *.o
	rm -f core core.*