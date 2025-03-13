# Nuggets Client

This directory contains the implementation for the client part of the Nuggets game.

## Overview

The client connects to a Nuggets game server, rendering the game map and allowing the player to move their character around the map to collect gold nuggets. The client can be run in two modes:
- As a player, who actively participates in the game
- As a spectator, who only watches the game

## Files

* `client.c` - The main client program, handling command-line arguments and the main loop
* `client_state.c` - Implementation of the client state module, maintaining the necessary client's information like port number and player's name
* `client_state.h` - Header for the client state module
* `display.c` - Implementation of the display module using ncurses
* `display.h` - Header for the display module
* `network.c` - Implementation of network communication with the server
* `network.h` - Header for the network module
* `Makefile` - For building the client executable

## Dependencies

The client depends on:
* `../support/` - Contains utility modules:
  * `message.h/c` - For network communication
  * `log.h/c` - For logging functionality
* `../grid/` - Contains the grid module for representing the game map
* `ncurses` library - For the terminal-based user interface

## Usage

```
./client hostname port [playername]
```

Where:
* `hostname` - Address of the server
* `port` - Port number of the server
* `playername` - Optional. If provided, client joins as a player; if omitted, client joins as a spectator

## Game Controls

When playing as a player, the following keys control movement:
* `h`, `H` - Move left
* `l`, `L` - Move right 
* `j`, `J` - Move down
* `k`, `K` - Move up
* `y`, `Y` - Move diagonally up-left
* `u`, `U` - Move diagonally up-right
* `b`, `B` - Move diagonally down-left
* `n`, `N` - Move diagonally down-right
* `Q`, `q` - Quit the game

Spectators can only use:
* `Q` - Quit spectating

## Building

To build the client:

```
make
```

To clean up:

```
make clean
```

## Implementation Notes

The client follows a modular design:

1. `client.c` serves as the main entry point, initializing all modules and managing the message loop
2. `client_state.c` maintains the client state, including player information and grid data
3. `display.c` handles the ncurses-based user interface, printing out the game map and status information
4. `network.c` manages communication with the server, with various message types