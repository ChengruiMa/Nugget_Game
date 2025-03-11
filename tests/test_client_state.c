#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "client_state.h"
#include "map.h"  // Using the map module as the grid implementation

void test_client_new_and_delete() {
    // Test for player client
    client_t* client = client_new("localhost", 1234, "Tester");
    assert(client != NULL);
    assert(client->isSpectator == false);
    assert(client->playerName != NULL);
    
    // Test for spectator client
    client_t* spec = client_new("localhost", 1234, NULL);
    assert(spec != NULL);
    assert(spec->isSpectator == true);
    assert(spec->playerName == NULL);
    
    client_delete(client);
    client_delete(spec);
    printf("test_client_new_and_delete passed\n");
}

void test_client_update_grid() {
    client_t* client = client_new("localhost", 1234, "Tester");
    assert(client != NULL);
    // Initially, grid should be NULL.
    assert(client->grid == NULL);
    
    // Update grid to 5 rows x 7 columns.
    int nrows = 5, ncols = 7;
    int success = client_update_grid(client, nrows, ncols);
    assert(success == 1);
    assert(client->grid != NULL);
    
    // Verify grid dimensions using functions from map.h.
    int rows = grid_getRows(client->grid);
    int cols = grid_getCols(client->grid);
    assert(rows == nrows && cols == ncols);
    
    client_delete(client);
    printf("test_client_update_grid passed\n");
}

void test_client_update_gold() {
    client_t* client = client_new("localhost", 1234, "Tester");
    assert(client != NULL);
    client_update_gold(client, 10, 50, 100);
    assert(client->lastGoldCollected == 10);
    assert(client->purse == 50);
    assert(client->remainingGold == 100);
    client_delete(client);
    printf("test_client_update_gold passed\n");
}

void test_client_handle_quit() {
    client_t* client = client_new("localhost", 1234, "Tester");
    assert(client != NULL);
    // Simply call the quit handler (which logs the quit reason)
    client_handle_quit(client, "Test quit reason");
    client_delete(client);
    printf("test_client_handle_quit passed\n");
}

int main() {
    test_client_new_and_delete();
    test_client_update_grid();
    test_client_update_gold();
    test_client_handle_quit();
    printf("All client_state tests passed!\n");
    return 0;
}
