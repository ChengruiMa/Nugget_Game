# Game Module - Nuggets

This directory contains the game state implementation for the Nuggets game.

## Overview

The game module maintains the core game state, handling players, spectators, the game grid, and gold distribution. It serves as the central component that coordinates all game elements.

## Files

* `game.c` - Implementation of the game state module
* `game.h` - Header file with function prototypes and type definitions

## Dependencies

The game module depends on:
* `../grid/` - For map representation
* `../player/` - For player management
* `../spectator/` - For spectator functionality

## Constants

* `MaxNameLength`: 50 characters
* `GoldTotal`: 250 nuggets in total
* `MaxPlayers`: 26 players (A-Z)
* `GoldMinNumPiles`: 10 minimum gold piles
* `GoldMaxNumPiles`: 30 maximum gold piles

## Usage

The game module is used by the server to create and manage the game state. It provides functions for:
* Creating a new game with a grid from a map file
* Adding and moving players
* Adding/Removing the spectator
* Handling gold collection
* Generating display strings for visualization

To build, simply 
```
make
```