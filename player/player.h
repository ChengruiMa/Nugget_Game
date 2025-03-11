/**
 * @file player.h
 * 
 * @brief This file provides the interface for the player module.
 * 
 * The player module is responsible for managing player state and operations.
 * 
 * @author Team 11 (11xers)
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdlib.h>

// include modules
#include "map.h" // ../map/map.h
#include "message.h" // ../support/message.h\

typedef struct player {
    char playerLetter; // A-Z identifying this player
    char* realName; // player's real name
    int purse; // how much gold this player has collected
    int row;                    // current row position in the grid
    int col;                    // current column position in the grid
    addr_t address;             // network address for sending messages
    grid_t* grid;               // the grid the player is on
    bool leftGame;              // true if player has left the game
} player_t;

/**
 * Create a new player.
 * 
 * @param realName: the player's real name
 * @param playerLetter: the player's letter
 * @param address: the player's network address
 * @param grid: the grid the player is on
 * 
 * @return a pointer to the new player
 */
player_t* player_new(char* realName, char playerLetter, addr_t address, grid_t* grid);

/**
 * Move a player to a new position.
 * 
 * @param player: the player to move
 * @param newRow: the new row position
 * @param newCol: the new column position
 */
void player_move(player_t* player, int newRow, int newCol);

/**
 * Add gold to a player's purse.
 * 
 * @param player: the player to add gold to
 * @param goldAmount: the amount of gold to add
 */
void player_addGold(player_t* player, int goldAmount);

/**
 * Free memory allocated for a player and delete the player struct
 * 
 * @param player: the player to free
 */
void player_delete(player_t* player);

/**
 * Get the player's letter.
 * 
 * @param player: the player to get the letter for
 * 
 * @return the player's letter
 */
char player_getLetter(player_t* player);

/**
 * Get the player's real name.
 * 
 * @param player: the player to get the real name for
 * 
 * @return a char* representing the player's real name
 */
char* player_getRealName(player_t* player);

/**
 * Get the player's gold (purse).
 * 
 * @param player: the player to get the gold for
 * 
 * @return the player's gold amount (in their purse)
 */
int player_getGold(player_t* player);

/**
 * Get the player's row.
 * 
 * @param player: the player to get the row for
 * 
 * @return the player's row position (x-value)
 */
int player_getRow(player_t* player);

/**
 * Get the player's column.
 * 
 * @param player: the player to get the column for
 * 
 * @return the player's column position (y-value)
 */
int player_getCol(player_t* player);

/**
 * Get the player's address.
 * 
 * @param player: the player to get the address for
 * 
 * @return the player's address
 */
addr_t player_getAddress(player_t* player);

/**
 * Get the player's grid (player-specific).
 * 
 * @param player: the player to get the grid for
 * 
 * @return the player's grid
 */
grid_t* player_getGrid(player_t* player);

/**
 * Get whether the player has left the game.
 * 
 * @param player: the player to check if they have left the game
 * 
 * @return true if the player has left the game, false otherwise
 */

#endif // PLAYER_H