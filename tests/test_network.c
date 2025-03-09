#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "client_state.h"
#include "network.h"
#include "map.h"      // For grid functions used in GRID message handling
#include "display.h"  // For display_initialize

// Depending on your build, display_initialize may require an actual terminal;
// for testing, we assume it succeeds.

void test_handle_ok_message() {
    client_t* client = client_new("localhost", 1234, "Tester");
    assert(client != NULL);
    // Initially, playerLetter should be '\0'
    assert(client->playerLetter == '\0');
    
    // Simulate an OK message.
    int cont = handle_ok_message(client, "OK A");
    assert(cont == 1);
    assert(client->playerLetter == 'A');
    
    client_delete(client);
    printf("test_handle_ok_message passed\n");
}

void test_handle_grid_message() {
    client_t* client = client_new("localhost", 1234, "Tester");
    assert(client != NULL);
    // Before the GRID message, grid should be NULL.
    assert(client->grid == NULL);
    
    // Simulate a GRID message, e.g., "GRID 5 7"
    int cont = handle_grid_message(client, "GRID 5 7");
    assert(cont == 1);
    assert(client->grid != NULL);
    int rows = grid_getRows(client->grid);
    int cols = grid_getCols(client->grid);
    assert(rows == 5 && cols == 7);
    
    client_delete(client);
    printf("test_handle_grid_message passed\n");
}

void test_handle_gold_message() {
    client_t* client = client_new("localhost", 1234, "Tester");
    assert(client != NULL);
    
    // Update gold via a GOLD message
    int cont = handle_gold_message(client, "GOLD 15 35 50");
    assert(cont == 1);
    assert(client->lastGoldCollected == 15);
    assert(client->purse == 35);
    assert(client->remainingGold == 50);
    
    client_delete(client);
    printf("test_handle_gold_message passed\n");
}

void test_handle_quit_message() {
    client_t* client = client_new("localhost", 1234, "Tester");
    assert(client != NULL);
    
    // handle_quit_message should return false to indicate quitting.
    int cont = handle_quit_message(client, "QUIT Testing quit");
    assert(cont == 0);
    
    client_delete(client);
    printf("test_handle_quit_message passed\n");
}

void test_handle_error_message() {
    client_t* client = client_new("localhost", 1234, "Tester");
    assert(client != NULL);
    
    // Test error message handling.
    int cont = handle_error_message(client, "ERROR Some error occurred");
    assert(cont == 1);
    
    client_delete(client);
    printf("test_handle_error_message passed\n");
}

int main() {
    test_handle_ok_message();
    test_handle_grid_message();
    test_handle_gold_message();
    test_handle_quit_message();
    test_handle_error_message();
    printf("All network tests passed!\n");
    return 0;
}
