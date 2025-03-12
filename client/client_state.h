/*
 * client_state.h - header file for client state module
 */

#ifndef __CLIENT_STATE_H
#define __CLIENT_STATE_H

#include <stdbool.h>
#include <ncurses.h>
#include "../support/message.h"


typedef struct grid grid_t; //declare beforehand to avoid circular dependency

//client state struct
typedef struct client {
  char* hostname;          //server hostname
  int port;                //server port
  char* playerName;        //player's name or NULL if spectator
  bool isSpectator;        //whether this client is a spectator
  char playerLetter;       //if player, the assigned letter
  int purse;               //player's gold collected
  int remainingGold;       //gold remaining in the game
  addr_t serverAddress;    //server's network address
  grid_t* grid;            //local representation of the game grid
  WINDOW* gameWindow;      //ncurses window for game display
  int lastGoldCollected;   //amount of gold collected in the last pickup
} client_t;

/*
 * Create a new client state structure
 * @param hostname server hostname
 * @param port server port
 * @param playerName player's name or NULL if spectator
 * @return new client state or NULL if error
 */
client_t* client_new(const char* hostname, int port, const char* playerName);

/*
 * Delete a client state structure and free all associated memory.
 * @param client client state to delete
 */
void client_delete(client_t* client);

/*
 * Update client's grid dimensions when we receive the GRID message.
 * @param client client state to update
 * @param nrows number of rows in the grid
 * @param ncols number of columns in the grid
 * @return true if successful, false if error
 */
bool client_update_grid(client_t* client, int nrows, int ncols);

/*
 * Update client's gold information when we receive the GOLD message.
 * @param client client state to update
 * @param collected amount of gold collected
 * @param purse player's gold collected
 * @param remaining gold remaining in the game
 */
void client_update_gold(client_t* client, int collected, int purse, int remaining);

/*
 * Process a QUIT message and prepare for client shutdown.
 * @param client client state to update
 * @param reason reason for quitting
 */
void client_handle_quit(client_t* client, const char* reason);

#endif // __CLIENT_STATE_H
