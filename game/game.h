/* 
* game.h - header file for CS50 Nuggets gameState module
*
* A game model that maintains the game state for the Nuggets game
* 
* Team 11, Winter 2025
* Mar 11, 2025
*/

#ifndef __GAMEMODEL_H
#define __GAMEMODEL_H

#include <stdio.h>
#include <stdbool.h>
#include "grid.h"

/**************** global constants ****************/
extern const int MaxNameLength;
extern const int GoldTotal;
extern const int MaxPlayers;
extern const int GoldMinNumPiles;
extern const int GoldMaxNumPiles;

/**************** global types ****************/
typedef struct game game_t;
typedef struct gold gold_t;

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
game_t* game_new(const char* filename);

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
bool game_add_player(game_t* game, const char* player_name, const char* address);

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

/**************** collect_gold ****************/
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
void collect_gold(game_t* game, player_t* player);

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
bool game_addSpectator(game_t* game, const char* from);

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
player_t* game_get_player(game_t* game, const char* address);

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

/**************** game_getNumPlayers ****************/
/* Get the number of players currently in the game.
* 
* Caller provides:
*   A valid game pointer
* We return:
*   The number of players in the game
*   -1 if game is NULL
*/
int game_getNumPlayers(game_t* game);

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

/**************** game_getTotalGold ****************/
/* Get the total amount of gold in the game.
* 
* Caller provides:
*   A valid game pointer
* We return:
*   The total amount of gold in the game
*   -1 if game is NULL
*/
int game_getTotalGold(game_t* game);

#endif // __GAMEMODEL_H