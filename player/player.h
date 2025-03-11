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

#endif // PLAYER_H