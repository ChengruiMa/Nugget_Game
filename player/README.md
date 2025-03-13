# Player Module - Nuggets

This directory contains the implementation for the player module in the Nuggets game.

## Overview

The player module manages all player-related functionality, including player state, movement, gold collection, and network communication. Each player has their own view of the game map and maintains memory of previously seen areas.

## Files

* `player.c` - Implementation of the player module
* `player.h` - Header file with function prototypes and type definitions

## Data Structure

The `player_t` structure contains all the necessary information about each player, including:
```
typedef struct player {
    char playerLetter;          // A-Z identifying this player
    char* realName;             // player's real name
    int purse;                  // how much gold this player has collected
    int row;                    // current row position in the grid
    int col;                    // current column position in the grid
    addr_t address;             // network address for sending messages
    grid_t* grid;               // the grid the player is on
    bool leftGame;              // true if player has left the game
} player_t;
```

## Functions

The module provides the following functions:
* `player_new()` - Create a new player with initial position
* `player_move()` - Move a player to a new position
* `player_addGold()` - Add collected gold to player's purse
* `player_delete()` - Free memory allocated for a player
* Various getters for player properties (location, gold, name, etc.)
* `player_sendMessage()` - Send a network message to the player

## Dependencies

The player module depends on:
* `../grid/` - For map representation and visibility calculations
* `../support/` - For network message handling

## Usage

The player module is used by the game module to:
* Create and place new players in the game
* Track player movements, positions and visibility
* Manage gold collection

To build, simply 
```
make
```