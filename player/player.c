/**
 * player.c — implementation of the player module
 * 
 * The player module is responsible for managing player state and operations.
 * 
 * @author Team 11 (11xers)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "player.h"
#include "message.h"
#include "grid.h"

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

player_t* player_new(char* realName, char playerLetter, addr_t address, grid_t* grid)
{
    player_t* player = malloc(sizeof(player_t));
    if (player == NULL)
    {
        fprintf(stderr, "Error creating new player\n");
        return NULL;
    }

    player->realName = realName;
    player->playerLetter = playerLetter;
    player->address = address;

    point_t* playerPos = grid_findEmptyRoomSpot(grid);
    if (playerPos == NULL) {
        fprintf(stderr, "Couldn't create new player struct: Error finding empty room spot for player\n");
        free(player);
        return NULL;
    }

    // set player position
    player->row = point_getRow(playerPos);
    player->col = point_getCol(playerPos);

    grid_t* playerGrid = grid_createPlayerGrid(grid); // initialize player grid (grid but with visibility information / memory of what has been seen for player)
    grid_calculateVisibility(playerGrid, playerPos); // calculate player grid visibility

    player->grid = playerGrid;
    player->leftGame = false;
    player->purse = 0;

    return player;
}

/**
 * Move a player to a new position.
 * 
 * @param player: the player to move
 * @param newRow: the new row position
 * @param newCol: the new column position
 */
void player_move(player_t* player, int newRow, int newCol)
{
    if (player == NULL)
    {
        fprintf(stderr, "Error moving player: player is NULL\n");
        return;
    }

    player->row = newRow;
    player->col = newCol;
}

/**
 * Add gold to a player's purse.
 * 
 * @param player: the player to add gold to
 * @param goldAmount: the amount of gold to add
 */
void player_addGold(player_t* player, int goldAmount)
{
    if (player == NULL)
    {
        fprintf(stderr, "Error adding gold to player: player is NULL\n");
        return;
    }

    player->purse += goldAmount;
}

/**
 * Free memory allocated for a player and delete the player struct
 * 
 * @param player: the player to free
 */
void player_delete(player_t* player)
{
    if (player == NULL)
    {
        fprintf(stderr, "Error freeing player: player is NULL\n");
        return;
    }

    free(player->realName); // free player's real name
    grid_delete(player->grid); // delete player's grid
    free(player); // free player struct itself
}

/**
 * Get the player's letter.
 * 
 * @param player: the player to get the letter for
 */
char player_getLetter(player_t* player)
{
    if (player == NULL)
    {
        fprintf(stderr, "Error getting player letter: player is NULL\n");
        return NULL;
    }

    return player->playerLetter;
}

/**
 * Get the player's real name.
 * 
 * @param player: the player to get the real name for
 */
char* player_getRealName(player_t* player)
{
    if (player == NULL)
    {
        fprintf(stderr, "Error getting player real name: player is NULL\n");
        return NULL;
    }

    return player->realName;
}

/**
 * Get the player's gold (purse).
 * 
 * @param player: the player to get the gold for
 */
int player_getGold(player_t* player)
{
    if (player == NULL)
    {
        fprintf(stderr, "Error getting player's gold (purse): player is NULL\n");
        return -1;
    }

    return player->purse;
}

/**
 * Get the player's row.
 * 
 * @param player: the player to get the row for
 */
int player_getRow(player_t* player)
{
    if (player == NULL)
    {
        fprintf(stderr, "Error getting player row: player is NULL\n");
        return -1;
    }

    return player->row;
}

/**
 * Get the player's column.
 * 
 * @param player: the player to get the column for
 */
int player_getCol(player_t* player)
{
    if (player == NULL)
    {
        fprintf(stderr, "Error getting player column: player is NULL\n");
        return -1;
    }

    return player->col;
}

/**
 * Get the player's address.
 * 
 * @param player: the player to get the address for
 */
addr_t player_getAddress(player_t* player)
{
    if (player == NULL)
    {
        fprintf(stderr, "Error getting player address: player is NULL\n");
        return message_noAddr();
    }

    return player->address;
}

/**
 * Get the player's grid (player-specific).
 * 
 * @param player: the player to get the grid for
 */
grid_t* player_getGrid(player_t* player)
{
    if (player == NULL)
    {
        fprintf(stderr, "Error getting player grid: player is NULL\n");
        return NULL;
    }

    return player->grid;
}

/**
 * Get whether a player has left the game.
 * 
 * @param player: the player to check
 */
bool player_hasLeftGame(player_t* player)
{
    if (player == NULL)
    {
        fprintf(stderr, "Error checking if player has left game: player is NULL\n");
        return false;
    }

    return player->leftGame;
}

/**
 * Send a message to a player.
 * 
 * @param player: the player to send the message to
 * @param message: the message to send
 */
void player_sendMessage(player_t* player, char* message)
{
    if (player == NULL || message == NULL)
    {
        fprintf(stderr, "Error sending message to player: player or message is NULL\n");
        return;
    }

    message_send(player->address, message);
}