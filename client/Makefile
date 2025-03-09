# Makefile for nuggets client

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
LIBS = -lncurses

SRCS = client.c client_state.c display.c network.c
OBJS = $(SRCS:.c=.o)
HDRS = client_state.h display.h network.h

PROG = client

SUPPORT_DIR = ../support
COMMON_DIR = ../common

SUPPORT_OBJS = $(SUPPORT_DIR)/message.o $(SUPPORT_DIR)/log.o
COMMON_OBJS = $(COMMON_DIR)/grid.o

.PHONY: all clean

all: $(PROG)

$(PROG): $(OBJS) $(SUPPORT_OBJS) $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

client.o: client.c client_state.h display.h network.h $(SUPPORT_DIR)/message.h $(SUPPORT_DIR)/log.h
client_state.o: client_state.c client_state.h $(SUPPORT_DIR)/log.h $(COMMON_DIR)/grid.h
display.o: display.c display.h client_state.h $(SUPPORT_DIR)/log.h
network.o: network.c network.h client_state.h display.h $(SUPPORT_DIR)/message.h $(SUPPORT_DIR)/log.h

%.o: %.c
	$(CC) $(CFLAGS) -I$(SUPPORT_DIR) -I$(COMMON_DIR) -c $<

clean:
	rm -f $(PROG) *~ *.o
	rm -f core core.*