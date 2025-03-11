# Game Logic Module Design

## Overview
The game logic (or game model) module is responsible for maintaining and updating the core state of the Nuggets game. It integrates with the provided modules (Map/Visibility, Player, etc.) and implements the key game mechanics:
- Loading the game map (grid) from a file.
- Managing the list of players.
- Detecting and processing gold collection.
- Enforcing movement rules (valid moves on the grid).
- Checking end-of-game conditions (when all gold is collected).
- Tying these together so that the server’s message-handling code can simply call these functions.

## Data Structures

### game_t
A central structure that holds:
- **grid_t\***: The master grid (loaded from the map file using the map module).
- **player_t\*\***: An array of pointers to players (created with the provided player module).
- **int num_players**: The number of players currently in the game.
- **int max_players**: Maximum allowed players (set to 4).
- **gold_pile_t\***: An array of gold piles, where each pile records its grid location and remaining nuggets.
- **int num_gold_piles**: The total number of gold piles (found by scanning the grid for gold spots).
- **int total_gold**: The sum of nuggets remaining in all piles.

### gold_pile_t
A structure that represents a single gold pile:
- **int row, col**: The location on the grid.
- **int amount**: The number of nuggets available (default set to 10).

## Key Functions

- **game_new(map_file)**  
  Opens the map file, creates a grid (using fixed dimensions for now, e.g., 10x20), loads the grid, scans for gold piles (each gold spot is assigned a default amount), initializes the player array, and computes the total gold.

- **game_add_player(game, player_name, address)**  
  Creates a new player (using the provided player module) and adds it to the game state.

- **game_move_player(game, player, new_row, new_col)**  
  Checks if the move is within bounds and if the destination is a valid room. If so, it calls the provided player_move function.

- **game_update_gold(game)**  
  For each player, if their current position matches a gold pile that still has nuggets, the gold is collected (added to the player's purse and removed from the pile). The grid cell is updated to remove the gold marker.

- **game_is_over(game)**  
  Returns true when all gold has been collected (i.e., total_gold is zero).

- **game_end(game)**  
  Notifies players that the game is over (here, by printing a message) and frees the game state.

## Integration with Server
The server’s message-handling code should be modified to call these functions:
- On receiving a `PLAY` command, call `game_add_player()`.
- On receiving a `KEY` command, call `game_move_player()`, then `game_update_gold()`, and finally check `game_is_over()`.
- If `game_is_over()` returns true, call `game_end()` to terminate the game.

This design cleanly separates the core game rules from the lower-level networking and display code.
