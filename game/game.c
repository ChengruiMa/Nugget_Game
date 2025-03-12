/* 
* game.c - implementation for CS50 Nuggets gameState module
*
* A game model that maintains the game state for the Nuggets game
* 
* Team 11, Winter 2025
* Mar 11, 2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>  // For inet_ntoa()
#include <netinet/in.h> // For sockaddr_in

#include "game.h"
#include "../grid/grid.h"
#include "../spectator/spectator.h"
#include "../player/player.h"

/**************** constants ****************/
const int MaxNameLength = 50;
const int GoldTotal = 250;
const int MaxPlayers = 26;
const int GoldMinNumPiles = 10;
const int GoldMaxNumPiles = 30;

/**************** types ****************/

/* Printing out the game state */
char* game_buildDisplayString(game_t* gameState);
char* game_buildPlayerDisplayString(game_t* gameState, player_t* player);

/**************** Local function declarations ****************/
static void calculateGoldDistribution(game_t* game);

/**************** game_new ****************/
game_t* game_new(FILE* map) 
{
    if (map == NULL) {
        fprintf(stderr, "Error: Invalid parameters in game_new\n");
        return NULL;
    }

    // Allocate memory for game structure
    game_t* game = malloc(sizeof(game_t));
    if (game == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for game\n");
        return NULL;
    }

    // Initialize game structure
    game->grid = grid_load(map);
    if (game->grid == NULL) {
        fprintf(stderr, "Error: Failed to load grid from file\n");
        free(game);
        return NULL;
    }

    // Initialize players array
    game->playersSeen = 0;
    game->players = calloc(MaxPlayers, sizeof(player_t*));
    if (game->players == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for players array\n");
        grid_delete(game->grid);
        free(game);
        return NULL;
    }

    // Initialize spectator
    game->spectator = NULL;

    // Initialize gold piles
    int numPiles = GoldMinNumPiles + (rand() % (GoldMaxNumPiles - GoldMinNumPiles + 1));
    game->numPiles = numPiles;
    game->goldPiles = calloc(numPiles, sizeof(gold_t*));
    if (game->goldPiles == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for gold piles\n");
        free(game->players);
        grid_delete(game->grid);
        free(game);
        return NULL;
    }

    // Initialize each gold pile
    for (int i = 0; i < numPiles; i++) {
        game->goldPiles[i] = malloc(sizeof(gold_t));
        if (game->goldPiles[i] == NULL) {
            // Clean up if allocation fails
            for (int j = 0; j < i; j++) {
                free(game->goldPiles[j]);
            }
            free(game->goldPiles);
            free(game->players);
            grid_delete(game->grid);
            free(game);
            return NULL;
        }
        game->goldPiles[i]->row = -1;
        game->goldPiles[i]->col = -1;
        game->goldPiles[i]->amount = 0;
        game->goldPiles[i]->player = NULL;
    }

    // Initialize gold remaining
    game->goldRemaining = GoldTotal;

    // Distribute gold piles
    game_distributeGold(game);

    return game;
}

/**************** game_delete ****************/
void game_delete(game_t* game) 
{
    if (game == NULL) {
        return;
    }

    // Free players
    if (game->players != NULL) {
        for (int i = 0; i < game->playersSeen; i++) {
            if (game->players[i] != NULL) {
                player_delete(game->players[i]);
            }
        }
        free(game->players);
    }

    // Free spectator
    if (game->spectator != NULL) {
        spectator_delete(game->spectator);
    }

    // Free grid
    if (game->grid != NULL) {
        grid_delete(game->grid);
    }

    // Free gold piles
    if (game->goldPiles != NULL) {
        for (int i = 0; i < game->numPiles; i++) {
            if (game->goldPiles[i] != NULL) {
                free(game->goldPiles[i]);
            }
        }
        free(game->goldPiles);
    }

    // Free game structure
    free(game);
}

/**************** calculateGoldDistribution ****************/
static void calculateGoldDistribution(game_t* game) 
{
    if (game == NULL || game->numPiles <= 0) {
        return;
    }

    int total = GoldTotal;
    int numPiles = game->numPiles;

    // initially ensure every pile has *at least* one piece of gold — to prevent empty piles in extreme edge cases
    for (int i = 0; i < numPiles; i++) {
        game->goldPiles[i]->amount = 1;
    }
    total -= numPiles; // remove the gauranteed 1 nugget/pile gold from the total

    // randomly distribute remaining gold across all piles
    while (total > 0) {
        int idx = rand() % numPiles;
        game->goldPiles[idx]->amount++;
        total--;
    }
}

/**************** game_distributeGold ****************/
void game_distributeGold(game_t* game) 
{
    if (game == NULL || game->goldPiles == NULL || game->grid == NULL) {
        fprintf(stderr, "Error: Invalid parameters in game_distributeGold\n");
        return;
    }

    // Calculate gold amounts for each pile
    calculateGoldDistribution(game);

    // Place each gold pile at a random empty room spot
    for (int i = 0; i < game->numPiles; i++) {
        // Find an empty room spot
        point_t* spot = grid_findEmptyRoomSpot(game->grid);
        if (spot == NULL) {
            fprintf(stderr, "Warning: Could not find empty spot for gold pile %d\n", i);
            continue;
        }

        // Set gold pile position
        game->goldPiles[i]->row = point_getRow(spot);
        game->goldPiles[i]->col = point_getCol(spot);
        game->goldPiles[i]->player = NULL;

        // Mark the spot on the grid
        grid_set(game->grid, game->goldPiles[i]->row, game->goldPiles[i]->col, '*');

        // Free the point
        point_delete(spot);
    }
}

/**************** game_add_player ****************/
bool game_addPlayer(game_t* game, player_t* newPlayer) 
{
    if (game == NULL || newPlayer == NULL) {
        return false;
    }

    // Check if the game is full
    if (game->playersSeen >= MaxPlayers) {
        return false;
    }

    // // Assign a letter to the player (A-Z based on number of players seen)
    // char letter = 'A' + game->playersSeen;

    // // Create a new player
    // player_t* newPlayer = player_new(player_name, letter, address, game->grid);
    // if (newPlayer == NULL) {
    //     return false;
    // }

    // Find a random empty spot for the player
    // point_t* spot = grid_findEmptyRoomSpot(game->grid);
    // if (spot == NULL) {
    //     player_delete(newPlayer);
    //     return false;
    // }

    // Set the player's position
    // player_move(newPlayer, point_getRow(spot), point_getCol(spot));
    // point_delete(spot);

    game->players[game->playersSeen] = newPlayer;
    game->playersSeen++;

    return true;
}

/**************** game_move_player ****************/
bool game_move_player(game_t* game, player_t* player, int new_row, int new_col) 
{
    if (game == NULL || player == NULL) {
        return false;
    }

    // get the game's master grid
    grid_t* grid = game_getGrid(game);

    // Check if the new position is valid
    if (new_row < 0 || new_row >= grid_getRows(grid) || 
        new_col < 0 || new_col >= grid_getCols(grid)) {
        return false;
    }

    // Check if the new position is a valid spot to move to (room or passage)
    if (!grid_isRoom(grid, new_row, new_col) && !grid_isPassage(grid, new_row, new_col)) {
        return false;
    }

    // Check if the new position is occupied by another player
    player_t* other = NULL;
    for (int i = 0; i < game->playersSeen; i++) {
        player_t* p = game->players[i];
        if (p != player && player_getRow(p) == new_row && player_getCol(p) == new_col) {
            other = p;
            break;
        }
    }

    // If there's another player, swap positions
    if (other != NULL) {
        int player_row = player_getRow(player);
        int player_col = player_getCol(player);
        
        player_move(player, new_row, new_col);
        player_move(other, player_row, player_col);
    } else {
        // Otherwise, just move the player
        player_move(player, new_row, new_col);
    }

    // Check if the player has collected gold
    game_collectGold(game, player);

    return true;
}

/**************** game_update_gold ****************/
void game_update_gold(game_t* game) 
{
    if (game == NULL) {
        return;
    }

    // Check each player for gold collection
    for (int i = 0; i < game->playersSeen; i++) {
        game_collectGold(game, game->players[i]);
    }
}

/**************** game_collectGold ****************/
int game_collectGold(game_t* game, player_t* player) 
{
    if (game == NULL || player == NULL) {
        return -1;
    }

    int player_row = player_getRow(player);
    int player_col = player_getCol(player);

    // Check all gold piles
    for (int i = 0; i < game->numPiles; i++) {
        gold_t* pile = game->goldPiles[i];
        
        fprintf(stderr, "PILE NUM %d WITH %d GOLD NUGGETS\n", i, pile->amount);
        // If the pile has gold and the player is on it
        if (pile != NULL && pile->player == NULL && pile->amount > 0 && 
            pile->row == player_row && pile->col == player_col) {

            int goldCollected = pile->amount;
            fprintf(stderr, "PLAYER IS ON GOLD PILE AND COLLECTED %d GOLD. ROW %d COL %d\n", goldCollected, pile->row, pile->col);
            // Add gold to player's purse
            player_addGold(player, goldCollected);
            
            // Update game state
            pile->amount = 0;
            pile->player = player;
            
            // Update grid (replace gold marker with empty spot)
            grid_set(game->grid, player_row, player_col, '.');

            return goldCollected; // make sure this isn't going to be zero
        }
    }

    return -1;
}

/**************** game_is_over ****************/
bool game_is_over(game_t* game) 
{
    if (game == NULL) {
        return false;
    }

    // The game is over when all gold has been collected
    return (game->goldRemaining <= 0);
}

/**************** game_end ****************/
void game_end(game_t* game) 
{
    if (game == NULL) {
        return;
    }

    printf("Game over! All gold has been collected.\n");
    
    // Print a summary of players and their gold
    printf("Final scores:\n");
    for (int i = 0; i < game->playersSeen; i++) {
        player_t* player = game->players[i];
        printf("%c\t%d\t%s\n", 
                player_getLetter(player), 
                player_getGold(player), 
                player_getRealName(player));
    }
    
    // Delete the game
    game_delete(game);
}

/**************** game_addSpectator ****************/
bool game_addSpectator(game_t* game, addr_t from) 
{
    if (game == NULL) {
        return false;
    }

    // If there's already a spectator, delete it first
    if (game->spectator != NULL) {
        spectator_delete(game->spectator);
        game->spectator = NULL;
    }

    // Create a new spectator
    game->spectator = spectator_new(from);
    
    return (game->spectator != NULL);
}

/**************** game_getPlayerFromAddress ****************/
player_t* game_getPlayerFromAddress(game_t* game, const addr_t address) 
{
    if (game == NULL) {
        return NULL;
    }

    // Search for the player with the given address
    fprintf(stderr, "HELLO players seen: %d\n", game->playersSeen);
    for (int i = 0; i < game->playersSeen; i++)
    {
        player_t *player = game->players[i];
        fprintf(stderr, "HELLO HERE\n");
        fprintf(stderr, "PLAYER IS: %s\n", player_getRealName(player));
        fprintf(stderr, "Player address: %s\n", inet_ntoa(player_getAddress(player).sin_addr));
        if (message_eqAddr(player->address, address))
        {
            return player;
        }
    }

    return NULL;
}

/**************** game_getGrid ****************/
grid_t* game_getGrid(game_t* game) 
{
    if (game == NULL) {
        return NULL;
    }
    return game->grid;
}

/**************** game_getSpectator ****************/
spectator_t* game_getSpectator(game_t* game) 
{
    if (game == NULL) {
        return NULL;
    }
    return game->spectator;
}

/**************** game_getGoldRemaining ****************/
int game_getGoldRemaining(game_t* game) 
{
    if (game == NULL) {
        return -1;
    }
    return game->goldRemaining;
}


/**************** game_buildDisplayString() ****************/
char* game_buildDisplayString(game_t* game) 
{
    if (game == NULL) {
        return NULL;
    }
    
    grid_t* grid = game_getGrid(game);
    if (grid == NULL || !grid->initialized) {
        return NULL;
    }
    
    // Calculate required size: each row + newline + null terminator
    size_t size = grid->nrows * (grid->ncols + 1) + 1;
    char* str = malloc(size);
    if (str == NULL) {
        return NULL;
    }
    
    str[0] = '\0';
    char line[grid->ncols + 1];  // +1 for null terminator
    
    // Create temporary grid to avoid modifying the original
    grid_t* tempGrid = grid_createPlayerGrid(grid);
    if (tempGrid == NULL) {
        free(str);
        return NULL;
    }
    
    // Add gold to the temporary grid
    gold_t** goldPiles = game->goldPiles;
    int numPiles = game->numPiles;
    for (int i = 0; i < numPiles; i++) {
        if (goldPiles[i] != NULL && goldPiles[i]->player == NULL) {
            int row = goldPiles[i]->row; 
            int col = goldPiles[i]->col;
            if (grid_get(grid, row, col) == '.') {
                grid_set(tempGrid, row, col, GRID_GOLD_SPOT);
            }
        }
    }
    
    // Add players to the temporary grid
    for (int id = 0; id < game->playersSeen; id++) {
        player_t* player = game->players[id];
        if (player != NULL) {
            int row = player_getRow(player);
            int col = player_getCol(player);
            char symbol = player_getLetter(player);
            grid_set(tempGrid, row, col, symbol);
        }
    }
    
    // Build string row by row
    for (int row = 0; row < grid->nrows; row++) {
        for (int col = 0; col < grid->ncols; col++) {
            line[col] = grid_get(tempGrid, row, col);
        }
        line[grid->ncols] = '\0';
        strcat(str, line);
        strcat(str, "\n");
    }
    
    // Clean up temporary grid
    grid_delete(tempGrid);
    
    return str;
}

/**************** grid_buildPlayerDisplayString() ****************/
char* game_buildPlayerDisplayString(game_t* game, player_t* player)
{
    if (game == NULL || player == NULL) {
        return NULL;
    }
    
    // Get player's grid
    grid_t* playerGrid = player_getGrid(player);
    if (playerGrid == NULL || !playerGrid->initialized || !playerGrid->hasMemory) {
        return NULL;
    }
    
    // Calculate required size: each row + newline + null terminator
    char* str = malloc(playerGrid->nrows * (playerGrid->ncols + 1) + 1);
    if (str == NULL) {
        return NULL;
    }
    
    str[0] = '\0';
    char line[playerGrid->ncols + 1];  // +1 for null terminator
    
    // Get player position for visibility calculations
    point_t* playerPos = point_new(player_getRow(player), player_getCol(player));
    if (playerPos == NULL) {
        free(str);
        return NULL;
    }
    
    // Update visibility from player's current position
    grid_calculateVisibility(playerGrid, playerPos);
    
    // Make a copy with the current player's grid
    grid_t* tempGrid = grid_createPlayerGrid(playerGrid);
    if (tempGrid == NULL) {
        point_delete(playerPos);
        free(str);
        return NULL;
    }
    
    // Add gold to the temporary grid if visible
    gold_t** goldPiles = game->goldPiles;
    int numPiles = game->numPiles;
    for (int i = 0; i < numPiles; i++) {
        if (goldPiles[i] != NULL && goldPiles[i]->player == NULL) {
            int row = goldPiles[i]->row; 
            int col = goldPiles[i]->col;
            if (grid_isPointVisible(playerGrid, row, col) && grid_get(playerGrid, row, col) == '.') {
                grid_set(tempGrid, row, col, GRID_GOLD_SPOT);
            }
        }
    }
    
    // Add players to the temporary grid if visible
    for (int id = 0; id < game->playersSeen; id++) {
        player_t* otherPlayer = game->players[id];
        if (otherPlayer != NULL) {
            int row = player_getRow(otherPlayer);
            int col = player_getCol(otherPlayer);
            
            // Only show if this spot is visible to the player
            if (row == player_getRow(player) && col == player_getCol(player)) {
                grid_set(tempGrid, row, col, '@');
            }
            else if (grid_isPointVisible(playerGrid, row, col)) {
                    char symbol = player_getLetter(otherPlayer);
                    grid_set(tempGrid, row, col, symbol);
            }
        }
    }
    
    // Build string row by row, using visibility information
    for (int row = 0; row < playerGrid->nrows; row++) {
        for (int col = 0; col < playerGrid->ncols; col++) {
            if (playerGrid->visible[row][col]) {
                // Spot is currently visible, use tempGrid content (with players and gold)
                line[col] = grid_get(tempGrid, row, col);
            } else {
                // Spot is not visible, use memory if previously seen
                char remembered = playerGrid->memory[row][col];
                if (remembered == GRID_GOLD_SPOT) {
                    // Gold that was seen before but is now out of sight is shown as an empty room
                    line[col] = '.';
                } else {
                    line[col] = remembered;
                }
            }
        }
        line[playerGrid->ncols] = '\0';
        strcat(str, line);
        strcat(str, "\n");
    }
    
    // Clean up
    grid_delete(tempGrid);
    point_delete(playerPos);
    
    return str;
}
