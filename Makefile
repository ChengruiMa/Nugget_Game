# Makefile for 'server' executable
#
# Team 11 (11xers), March 2025

# Executable
PROG = server
OBJS = server.o
LIBS = $(NLIB) $(SLIB)
RLIBS = $(SLIB) -lm

CDIR = client

# Dependency Libraries
NLIB = nuglib.a # Nuggets Library (all modules used in the game)
S = support
SLIB = $(S)/$(S).a # Support Library

# Compiler and Compilation flags
CCFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$(SLIB) -I$(NLIB)
CC = gcc
MAKE = make
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# Uncomment the following to turn on verbose memory logging
# TESTING=-DMEMTEST # Uncomment to turn on verbose memory logging

# Phony targets
.PHONY: all test clean client

# Default target
all: $(RLIBS) $(PROG) $(CDIR)

$(PROG): $(OBJS) $(RLIBS)
	$(CC) $(CCFLAGS) $^ $(NLIB) $(RLIBS) -o $@
	rm -rf $(OBJS) $(NLIB)

# Dependency Libraries
$(NLIB): grid/grid.o player/player.o spectator/spectator.o game/game.o
	ar cr $(NLIB) $^
# rm -rf $^

$(SLIB):
	$(MAKE) -C $(S)

# Dependency Objects
server.o: server.c $(NLIB)

grid.o: grid/grid.h

player.o: $(S)/message.h ./grid/grid.h player/player.h

spectator.o: $(S)/message.h spectator/spectator.h

game.o: $(S)/message.h grid/grid.h player/player.h spectator/spectator.h game/game.h

# Client make (call make in client directory)
$(CDIR):
	$(MAKE) -C $(CDIR)

# Valgrind
valgrind: $(PROG)
	$(VALGRIND) ./$(PROG) ./maps/main.txt

# Clean up
clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f $(LIBS)
	$(MAKE) -C $(S) clean
	$(MAKE) -C $(CDIR) clean