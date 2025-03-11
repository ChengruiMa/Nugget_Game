#include <stdio.h>
#include <assert.h>
#include "client_state.h"
#include "display.h"
#include "map.h"  // For grid functions

// Minimal test for display initialization and cleanup.
// Note: Since display functions use ncurses, these tests assume
// that the terminal environment is appropriate for ncurses calls.

void test_display_initialize_and_cleanup() {
    client_t* client = client_new("localhost", 1234, "Tester");
    assert(client != NULL);
    
    // Create a dummy grid for the client.
    client->grid = grid_new(10, 20);
    assert(client->grid != NULL);
    
    // Call display_initialize; assume it returns true (nonzero).
    int init = display_initialize(client);
    assert(init == 1);
    
    // Optionally, you could check display_check_size here if implemented.
    // Call display_status with a sample message.
    display_status(client, "Test status message");
    
    // Clean up the display.
    display_cleanup(client);
    
    client_delete(client);
    printf("test_display_initialize_and_cleanup passed\n");
}

int main() {
    test_display_initialize_and_cleanup();
    printf("All display tests passed!\n");
    return 0;
}
