#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <stdbool.h>
#include "map.h"
#include "player.h"

/* Opaque game state type */
typedef struct game game_t;

/* Initialize a new game state from a map file.
   Returns a pointer to the game state, or NULL on error. */
game_t* game_new(const char* map_file);

/* Free the game state and all its resources. */
void game_free(game_t* game);

/* Add a new player to the game.
   Returns true if successful, false otherwise. */
bool game_add_player(game_t* game, const char* player_name, const char* address);

/* Attempt to move a player to the new position.
   Returns true if the move is successful, false otherwise. */
bool game_move_player(game_t* game, player_t* player, int new_row, int new_col);

/* Update the gold collection state.
   Checks if any player has stepped on a gold pile and collects the gold. */
void game_update_gold(game_t* game);

/* Check if the game is over (i.e., all gold has been collected).
   Returns true if game is over, false otherwise. */
bool game_is_over(game_t* game);

/* End the game (notify players, cleanup, etc.). */
void game_end(game_t* game);

#endif // GAMEMODEL_H
