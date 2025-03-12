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
#include "game.h"
#include "grid.h"
#include "spectator.h"
#include "player.h"

/**************** constants ****************/
const int MaxNameLength = 50;
const int GoldTotal = 250;
const int MaxPlayers = 26;
const int GoldMinNumPiles = 10;
const int GoldMaxNumPiles = 30;

/**************** types ****************/
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

/**************** function declarations ****************/
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
    game->numPlayers = 0;
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
        for (int i = 0; i < game->numPlayers; i++) {
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
    int remaining = game->numPiles;

    // For each pile (except the last), assign a random portion of the remaining gold
    for (int i = 0; i < game->numPiles - 1; i++) {
        // The last pile will get all remaining gold
        if (remaining == 1) {
            game->goldPiles[i]->amount = total;
            break;
        }

        // Assign a random amount of gold, but have at least 1 gold per pile
        int max = total - remaining + 1; 
        int amount = 1 + (rand() % max);
        
        game->goldPiles[i]->amount = amount;
        total -= amount;
        remaining--;
    }

    // The last pile gets any remaining gold
    if (remaining > 0) {
        game->goldPiles[game->numPiles - 1]->amount = total;
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
    point_t* spot = grid_findEmptyRoomSpot(game->grid);
    if (spot == NULL) {
        player_delete(newPlayer);
        return false;
    }

    // Set the player's position
    player_move(newPlayer, point_getRow(spot), point_getCol(spot));
    point_delete(spot);

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

    // Check if the new position is valid
    if (new_row < 0 || new_row >= grid_getRows(game->grid) || 
        new_col < 0 || new_col >= grid_getCols(game->grid)) {
        return false;
    }

    // Check if the new position is a valid spot to move to (room or passage)
    char cell = grid_get(game->grid, new_row, new_col);
    if (!grid_isRoom(grid, new_row, new_col) && !grid_isPassage(grid, new_row, new_col)) {
        return false;
    }

    // Check if the new position is occupied by another player
    player_t* other = NULL;
    for (int i = 0; i < game->numPlayers; i++) {
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
    for (int i = 0; i < game->numPlayers; i++) {
        game_collectGold(game, game->players[i]);
    }
}

/**************** game_collectGold ****************/
int game_collectGold(game_t* game, player_t* player) 
{
    if (game == NULL || player == NULL) {
        return;
    }

    int player_row = player_getRow(player);
    int player_col = player_getCol(player);

    // Check all gold piles
    for (int i = 0; i < game->numPiles; i++) {
        gold_t* pile = game->goldPiles[i];
        
        // If the pile has gold and the player is on it
        if (pile != NULL && pile->player == NULL && pile->amount > 0 && 
            pile->row == player_row && pile->col == player_col) {

            int goldCollected = pile->amount;
            
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
    for (int i = 0; i < game->numPlayers; i++) {
        player_t* player = game->players[i];
        printf("%c\t%d\t%s\n", 
                player_getLetter(player), 
                player_getGold(player), 
                player_getName(player));
    }
    
    // Delete the game
    game_delete(game);
}

/**************** game_addSpectator ****************/
bool game_addSpectator(game_t* game, addr_t from) 
{
    if (game == NULL || from == NULL) {
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
    if (game == NULL || address == NULL) {
        return NULL;
    }

    // Search for the player with the given address
    for (int i = 0; i < game->playersSeen; i++)
    {
        player_t *player = game->players[i];
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