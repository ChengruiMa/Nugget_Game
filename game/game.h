/* 
* game.h - header file for CS50 Nuggets gameState module
*
* A game model that maintains the game state for the Nuggets game
* 
* Team 11, Winter 2025
* Mar 11, 2025
*/

#ifndef __GAME_H
#define __GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "../grid/grid.h"
#include "../spectator/spectator.h"
#include "../support/message.h"
#include "../player/player.h"

/**************** global constants ****************/
extern const int MaxNameLength;
extern const int GoldTotal;
extern const int MaxPlayers;
extern const int GoldMinNumPiles;
extern const int GoldMaxNumPiles;

/**************** global types ****************/
// typedef struct game game_t;
// typedef struct gold gold_t;

typedef struct gold {
    int row;
    int col;
    int amount;
    player_t* player;  // The player who collected this gold (NULL if not collected)
} gold_t;

typedef struct game {
    grid_t* grid;             // Master grid (loaded from the map file)
    player_t** players;       // Array of pointers to players
    spectator_t* spectator;   // Current spectator (if any)
    gold_t** goldPiles;       // Array of gold piles
    
    int playersSeen;          // Total number of players seen (joined and left)
    int numPiles;             // Number of gold piles
    int goldRemaining;        // Total gold remaining on the grid
} game_t;

/**************** Public functions ****************/

/**************** game_new ****************/
/* Create a new game for a given map file with a maximum number of players.
* 
* Caller provides:
*   A valid String representing the filename
* We return:
*   A new game structure initialized with the given map and settings
*   NULL if any error occurred
* Notes:
*   The caller must later call game_delete
*/
game_t* game_new(FILE* map);

/**************** game_delete ****************/
/* Delete the game, freeing all allocated memory.
* 
* Caller provides:
*   A valid game pointer
* Notes:
*   Frees all internal memory allocated for the game
*   Does nothing if game is NULL
*/
void game_delete(game_t* game);

/**************** game_distributeGold ****************/
/* Distribute gold piles across the map at the start of the game.
* 
* Caller provides:
*   A valid game pointer
* Notes:
*   Creates gold piles in random empty spots on the map
*   Sets the amount of gold per pile to random values
*   Does nothing if game is NULL
*/
void game_distributeGold(game_t* game);

/**************** game_add_player ****************/
/* Add a new player to the game.
* 
* Caller provides:
*   A valid game pointer
*   A player name (non-NULL string)
*   A player address (non-NULL string)
* We return:
*   true if the player was successfully added
*   false if any error occurred or if the game is already full
* Notes:
*   The player is given a random valid position on the map
*/
bool game_addPlayer(game_t* game, player_t* newPlayer);

/**************** game_move_player ****************/
/* Move a player to a new position on the map.
* 
* Caller provides:
*   A valid game pointer
*   A valid player pointer
*   New row and column coordinates
* We return:
*   true if the player was successfully moved
*   false if any error occurred or if the move is invalid
*/
bool game_move_player(game_t* game, player_t* player, int new_row, int new_col);

/**************** game_update_gold ****************/
/* Update gold collection for all players.
* 
* Caller provides:
*   A valid game pointer
* Notes:
*   Checks each player's position against gold piles
*   Updates player's purse and the game state if gold is collected
*   Does nothing if game is NULL
*/
void game_update_gold(game_t* game);

/**************** game_collectGold ****************/
/* Collect gold for a specific player.
* 
* Caller provides:
*   A valid game pointer
*   A valid player pointer
* Notes:
*   Checks the player's position against gold piles
*   Updates player's purse and the game state if gold is collected
*   Does nothing if game or player is NULL
*/
int game_collectGold(game_t* game, player_t* player);

/**************** game_is_over ****************/
/* Check if the game is over (all gold collected) by checking the goldRemaining field in game.
* 
* Caller provides:
*   A valid game pointer
* We return:
*   true if all gold has been collected
*   false otherwise
*/
bool game_is_over(game_t* game);

/**************** game_end ****************/
/* End the game and clean up.
* 
* Caller provides:
*   A valid game pointer
* Notes:
*   Prints game over message
*   Frees all memory
*/
void game_end(game_t* game);

/**************** game_addSpectator ****************/
/* Add a spectator to the game (if there's an existing one, kick the current one out)
* 
* Caller provides:
*   A valid game pointer
*   A spectator address (non-NULL string)
* We return:
*   true if the spectator was successfully added
*   false if any error occurred
*/
bool game_addSpectator(game_t* game, addr_t from);

/**************** game_get_player ****************/
/* Get a player based on their address.
* 
* Caller provides:
*   A valid game pointer
*   A player address (non-NULL string)
* We return:
*   A pointer to the player if found
*   NULL if not found or if any error occurred
*/
player_t* game_getPlayerFromAddress(game_t* game, const addr_t address);

/**************** game_getGrid ****************/
/* Get the master grid of the game.
* 
* Caller provides:
*   A valid game pointer
* We return:
*   A pointer to the game's grid
*   NULL if game is NULL
*/
grid_t* game_getGrid(game_t* game);

/**************** game_getSpectator ****************/
/* Get the spectator of the game.
* 
* Caller provides:
*   A valid game pointer
* We return:
*   A pointer to the game's spectator
*   NULL if game is NULL or no spectator exists
*/
spectator_t* game_getSpectator(game_t* game);

/**************** game_getGoldRemaining ****************/
/* Get the amount of gold remaining in the game.
* 
* Caller provides:
*   A valid game pointer
* We return:
*   The amount of gold remaining to be collected
*   -1 if game is NULL
*/
int game_getGoldRemaining(game_t* game);

/* Build a string representation of the game state for display
 *
 * Caller provides:
 *   Pointer to a game state structure
 * We return:
 *   Pointer to a newly allocated string showing the game state, or NULL on error
 * Notes:
 *   Includes all players and uncollected gold on the grid (to check if a gold is collected, we check if the gold's player_t struct is NULL)
 *   Caller must free the returned string when done
 */
char* game_buildDisplayString(game_t* gameState);

/* Build a string representation of what a specific player can see
 *
 * Caller provides:
 *   Pointer to a game state structure and pointer to a player
 * We return:
 *   Pointer to a newly allocated string showing what the player can see, or NULL on error
 * Notes:
 *   Shows only what is visible to that player plus remembered areas
 *   Caller must free the returned string when done
 */
char* game_buildPlayerDisplayString(game_t* gameState, player_t* player);

#endif // __GAMEMODEL_H
