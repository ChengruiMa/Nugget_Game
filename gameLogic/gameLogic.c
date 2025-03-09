#include "gamemodel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Define a structure for a gold pile */
typedef struct gold_pile {
    int row;
    int col;
    int amount; // number of nuggets remaining in this pile
} gold_pile_t;

/* Define the game state structure */
struct game {
    grid_t* grid;         // Master grid (loaded from the map file)
    player_t** players;   // Array of pointers to players
    int num_players;      // Current number of players
    int max_players;      // Maximum allowed players (e.g., 4)
    gold_pile_t* gold_piles; // Array of gold piles
    int num_gold_piles;   // Total number of gold piles
    int total_gold;       // Sum of nuggets remaining across all piles
};

#define DEFAULT_GOLD_AMOUNT 10

/* Helper function to scan the grid for gold spots ('*') and record their positions.
   Returns an array of gold_pile_t and sets out_num_gold to the count. */
static gold_pile_t* scan_for_gold(grid_t* grid, int* out_num_gold) {
    if (!grid || !out_num_gold) return NULL;
    int rows = grid_getRows(grid);
    int cols = grid_getCols(grid);
    int count = 0;
    // First pass: count gold cells.
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (grid_get(grid, r, c) == GRID_GOLD_SPOT) {
                count++;
            }
        }
    }
    *out_num_gold = count;
    if (count == 0) return NULL;
    
    // Allocate array for gold piles.
    gold_pile_t* piles = malloc(sizeof(gold_pile_t) * count);
    if (!piles) return NULL;
    
    int index = 0;
    // Second pass: record positions and assign default gold amount.
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (grid_get(grid, r, c) == GRID_GOLD_SPOT) {
                piles[index].row = r;
                piles[index].col = c;
                piles[index].amount = DEFAULT_GOLD_AMOUNT;
                index++;
            }
        }
    }
    return piles;
}

game_t* game_new(const char* map_file) {
    if (!map_file) return NULL;
    
    FILE* fp = fopen(map_file, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open map file: %s\n", map_file);
        return NULL;
    }
    
    int nrows = 10;
    int ncols = 20;
    
    game_t* game = malloc(sizeof(game_t));
    if (!game) {
        fclose(fp);
        return NULL;
    }
    
    game->grid = grid_new(nrows, ncols);
    if (!game->grid) {
        free(game);
        fclose(fp);
        return NULL;
    }
    
    if (!grid_load(game->grid, fp)) {
        fprintf(stderr, "Error: Failed to load grid from file: %s\n", map_file);
        grid_delete(game->grid);
        free(game);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    
    // Initialize players array.
    game->max_players = 4;
    game->num_players = 0;
    game->players = malloc(sizeof(player_t*) * game->max_players);
    if (!game->players) {
        grid_delete(game->grid);
        free(game);
        return NULL;
    }
    
    // Scan grid for gold piles.
    game->gold_piles = scan_for_gold(game->grid, &game->num_gold_piles);
    if (game->gold_piles == NULL) {
        game->num_gold_piles = 0;
        game->total_gold = 0;
    } else {
        game->total_gold = 0;
        for (int i = 0; i < game->num_gold_piles; i++) {
            game->total_gold += game->gold_piles[i].amount;
        }
    }
    
    return game;
}

void game_free(game_t* game) {
    if (!game) return;
    if (game->players) {
        for (int i = 0; i < game->num_players; i++) {
            if (game->players[i]) {
                player_delete(game->players[i]);
            }
        }
        free(game->players);
    }
    if (game->grid) {
        grid_delete(game->grid);
    }
    if (game->gold_piles) {
        free(game->gold_piles);
    }
    free(game);
}

bool game_add_player(game_t* game, const char* player_name, const char* address) {
    if (!game || !player_name || !address) return false;
    if (game->num_players >= game->max_players) return false;
    
    /* Create new player using the provided player_new.
       Assumes player_new takes a name, address, and grid pointer. */
    player_t* newPlayer = player_new(player_name, address, game->grid);
    if (!newPlayer) return false;
    
    game->players[game->num_players++] = newPlayer;
    return true;
}

bool game_move_player(game_t* game, player_t* player, int new_row, int new_col) {
    if (!game || !player) return false;
    int rows = grid_getRows(game->grid);
    int cols = grid_getCols(game->grid);
    if (new_row < 0 || new_row >= rows || new_col < 0 || new_col >= cols) return false;
    if (!grid_isRoom(game->grid, new_row, new_col)) return false;
    
    int result = player_move(player, game->grid, new_row, new_col);
    return (result == 1);
}

void game_update_gold(game_t* game) {
    if (!game) return;
    // For each player, check if their position matches any gold pile that still has gold.
    for (int i = 0; i < game->num_players; i++) {
        player_t* p = game->players[i];
        for (int j = 0; j < game->num_gold_piles; j++) {
            if (game->gold_piles[j].amount > 0 &&
                p->row == game->gold_piles[j].row &&
                
                p->col == game->gold_piles[j].col) {

                p->purse += game->gold_piles[j].amount;
                game->total_gold -= game->gold_piles[j].amount;
                game->gold_piles[j].amount = 0;
                grid_set(game->grid, p->row, p->col, GRID_EMPTY_SPOT);
            }
        }
    }
}

bool game_is_over(game_t* game) {
    if (!game) return false;
    return (game->total_gold <= 0);
}

void game_end(game_t* game) {
    printf("Game over! All gold has been collected.\n");
    game_free(game);
}
