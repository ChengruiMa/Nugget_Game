#include <stdio.h>
#include <assert.h>
#include "player.h"
#include "map.h"  // Changed from "grid.h" to "map.h"

void test_player_new_and_delete() {
    player_t* player = player_new("Aral");
    assert(player != NULL);
    assert(player->purse == 0);
    // For this test, we assume the initial row and col are 0.
    assert(player->row == 0 && player->col == 0);
    player_delete(player);
    printf("test_player_new_and_delete passed\n");
}

void test_player_move() {
    grid_t* grid = grid_new(5, 5);
    player_t* player = player_new("Neal");
    // Move the player to (3, 3)
    int success = player_move(player, grid, 3, 3);
    assert(success == 1);
    assert(player->row == 3 && player->col == 3);
    player_delete(player);
    grid_delete(grid);
    printf("test_player_move passed\n");
}

int main() {
    test_player_new_and_delete();
    test_player_move();
    printf("All player tests passed!\n");
    return 0;
}
