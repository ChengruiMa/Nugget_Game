# Nuggets Game

## Overview

Nuggets is a multi-player game where players explore a map, collect gold nuggets, and compete to become the wealthiest. The game consists of a server that hosts the game and at most 26 clients that connect to the server to play (with the addition of 1 spectator).

## Game Description

In Nuggets, players navigate through a maze-like map system represented by ASCII characters. The cave contains rooms (`.`), passages (`#`), walls (`+`, `-`, `|`), and gold nuggets (`*`). Players are identified by uppercase letters, and can see only a limited portion of the cave (what's in their line of sight). As players move around, they collect gold nuggets. The game ends when all gold has been collected, and the player with the most gold wins.

## Repository Structure

The repository is organized into several modules, each responsible for a specific aspect of the game:

```
nuggets/
├── client/             # Client implementation
├── server.c            # Main server program
├── game/               # Game state module
├── grid/               # Grid and visibility module
├── player/             # Player module
├── spectator/          # Spectator module
├── support/            # Support modules (message passing, logging)
├── maps/               # Map files for the game
```

## Modules

### Server (`server.c`)

The main server program that coordinates the game, handles network communication, processes client messages, and maintains the game state.

Key features:
- Initializes the game with a map file
- Processes client messages (PLAY, KEY, SPECTATE, QUIT)
- Handles player movements and gold collection
- Updates clients with their views of the game
- Manages game termination and final scoring

### Client Module

The client connects to a Nuggets server, renders the game display, and handles user input.

Key components:
- Network communication with the server
- Display rendering using ncurses
- Input handling for player movements
- Game state management on the client side

### Game Module

Manages the overall game state, including players, spectators, the game grid, and gold distribution.

Key features:
- Game initialization and clean-up
- Player and spectator management
- Gold distribution and collection
- Visibility and display string generation

### Grid Module

Represents the game map and handles visibility calculations.

Key features:
- 2D grid representation with cells
- Line-of-sight visibility calculations
- Memory of previously seen areas
- Loading maps from files

### Player Module

Manages player-specific functionality and state.

Key features:
- Player creation and movement
- Gold collection and tracking
- Player-specific view of the game map
- Network communication with client

### Spectator Module

Handles spectator functionality, allowing users to watch the game without participating.

Key features:
- Spectator creation and management
- Network communication with spectator client
- Complete view of the game map

### Support Modules

Utility modules that's provided to us

Key components:
- Message passing for network communication
- Logging for debugging and error reporting

## Game Protocol

The game uses a simple text-based protocol for communication between the server and clients:

### Client to Server Messages:
- `PLAY [name]` - Request to join as a player
- `SPECTATE` - Request to join as a spectator
- `KEY [k]` - Send a keystroke command
- `QUIT` - Request to leave the game

### Server to Client Messages:
- `OK [letter]` - Acknowledge player join
- `GRID [nrows] [ncols]` - Send grid dimensions
- `GOLD [n] [p] [r]` - Update gold status
- `DISPLAY\n[map]` - Send display update
- `QUIT [message]` - Notify of game end

## Building and Running

### Building
```
make
```

### Running the Server
```
./server [map] [seed]
```
Where:
- `map` is the path to a map file
- `seed` (optional) is a random seed for gold distribution

### Running the Client
```
./client hostname port [playername]
```
Where:
- `hostname` is the server's address
- `port` is the server's port
- `playername` (optional) is the player's name (if omitted, joins as spectator)

## Game Controls

### Player Controls:
- `h` - Move left
- `l` - Move right
- `j` - Move down
- `k` - Move up
- `y` - Move diagonally up-left
- `u` - Move diagonally up-right
- `b` - Move diagonally down-left
- `n` - Move diagonally down-right
- Uppercase versions (e.g., `H`, `L`) move in that direction until hitting a wall
- `Q` - Quit the game

In addition to the requirement spec, we also allow the following keys for simplicity. Note that capitalizing these `WASD` keys won't TP the player across the map.:
- `a` - Move left
- `d` - Move right
- `s` - Move down
- `w` - Move up

### Spectator Controls:
- `Q` - Quit spectating

## Implementation Details

### Grid Representation
The game map is represented as a 2D grid of characters, where:
- `.` represents an empty room space
- `#` represents a passage
- `+`, `-`, `|` represent walls
- `*` represents a gold nugget
- `@` represents the player's own position
- Uppercase letters represent other players

### Visibility
Players can only see portions of the map that are within their line of sight, which is calculated using a line of sight algorithm. Areas that were previously seen but are no longer visible are remembered but shown differently (if it's a gold that wasn't picked up or another player, it'll be remembered as a normal game spot).

### Gold Distribution
Gold is randomly distributed throughout the map in piles of random amounts. When a player moves onto a pile, they collect all the gold in that pile. The game ends when all gold has been collected.