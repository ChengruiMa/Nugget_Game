/*
 * client_state.c - implementation of client state module
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../support/log.h"
#include "../common/grid.h"
#include "client_state.h"

/*
 * Create a new client state structure
 * @param hostname server hostname
 * @param port server port
 * @param playerName player's name or NULL if spectator
 * @return new client state or NULL if error
 */
client_t* client_new(const char* hostname, int port, const char* playerName)
{
    if (hostname == NULL || port <= 0) { //validate params
        log_v("Invalid parameters to client_new");
        return NULL;
    }
    
    client_t* client = malloc(sizeof(client_t)); //allocate mem for client structure
    if (client == NULL) {
        log_v("There was an erro – failed to allocate memory for client structure");
        return NULL;
    }
    
    client->hostname = strdup(hostname); //copy hostname
    if (client->hostname == NULL) {
        log_v("There was an error – failed to copy hostname");
        free(client);
        return NULL;
    }
    
    client->port = port; //set port
    
    if (playerName == NULL) { //determine if spectator or player
        client->isSpectator = true;
        client->playerName = NULL;
    } else {
        client->isSpectator = false;
        client->playerName = strdup(playerName);
        if (client->playerName == NULL) {
            log_v("There was an error – failed to copy player name");
            free(client->hostname);
            free(client);
            return NULL;
        }
    }
    
    client->playerLetter = '\0'; //initialize other fields
    client->purse = 0;
    client->remainingGold = 0;
    client->serverAddress = message_noAddr();
    client->grid = NULL;
    client->gameWindow = NULL;
    client->lastGoldCollected = 0;
    
    return client;
}

/*
 * Delete a client state structure and free all associated memory
 * @param client client state to delete
 */
void client_delete(client_t* client)
{
    if (client == NULL) {
        return;
    }
    
    if (client->hostname != NULL) { //free allocated mem
        free(client->hostname);
    }
    
    if (client->playerName != NULL) {
        free(client->playerName);
    }
    
    if (client->grid != NULL) {
        grid_delete(client->grid);
    }
    
    free(client); //free client structure itself    
}

/*
 * Update client's grid dimensions when we receive the GRID message
 * @param client client state to update
 * @param nrows number of rows in the grid
 * @param ncols number of columns in the grid
 * @return true if successful, false if error
 */
bool client_update_grid(client_t* client, int nrows, int ncols)
{
    if (client == NULL || nrows <= 0 || ncols <= 0) { //validate params
        return false;
    }
    
    if (client->grid != NULL) { //delete existing grid if it exists
        grid_delete(client->grid);
    }
    
    client->grid = grid_new(nrows, ncols); //create new grid
    if (client->grid == NULL) {
        log_v("There was an error – failed to create grid");
        return false;
    }
    
    return true;
}

/*
 * Update client's gold information when we receive the GOLD message
 * @param client client state to update
 * @param collected amount of gold collected
 * @param purse player's gold collected
 * @param remaining gold remaining in the game
 * @return true if successful, false if error
 */
void client_update_gold(client_t* client, int collected, int purse, int remaining)
{
    if (client == NULL) { //validate params
        return;
    }
    
    client->lastGoldCollected = collected;
    client->purse = purse;
    client->remainingGold = remaining;
}

/*
 * Process a QUIT message and prepare for client shutdown
 * @param client client state to update
 * @param reason reason for quitting
 */
void client_handle_quit(client_t* client, const char* reason)
{
    if (client == NULL || reason == NULL) { //validate params
        return;
    }
    
    log_v("Quitting: %s", reason); //log quit reason
}