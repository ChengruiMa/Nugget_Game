/*
 * network.h - header file for network communication module
 *
 */

#ifndef __NETWORK_H
#define __NETWORK_H

#include <stdbool.h>
#include "../support/message.h"
#include "client_state.h"

/*
 * Send a PLAY message to the server
 * @param client client state to send message from
 */
void send_play_message(client_t* client);

/*
 * Send a SPECTATE message to the server
 * @param client client state to send message from
 */
void send_spectate_message(client_t* client);

/*
 * Send a KEY message to the server
 * @param client client state to send message from
 * @param key key pressed by the user
 */
void send_key_message(client_t* client, char key);

/*
 * Handle an OK message from the server
 * @param client client state to update
 * @param message message from server
 * @return true to continue the message loop, false to exit
 */
bool handle_ok_message(client_t* client, const char* message);

/*
 * Handle a GRID message from the server
 * @param client client state to update
 * @param message message from server
 * @return true to continue the message loop, false to exit
 */
bool handle_grid_message(client_t* client, const char* message);

/*
 * Handle a GOLD message from the server
 * @param client client state to update
 * @param message message from server
 * @return true to continue the message loop, false to exit
 */
bool handle_gold_message(client_t* client, const char* message);

/*
 * Handle a DISPLAY message from the server
 * @param client client state to update
 * @param message message from server
 * @return true to continue the message loop, false to exit
 */
bool handle_display_message(client_t* client, const char* message);

/*
 * Handle a QUIT message from the server
 * @param client client state to update
 * @param message message from server
 * @return false to exit the message loop
 */
bool handle_quit_message(client_t* client, const char* message);

/*
 * Handle an ERROR message from the server
 * @param client client state to update
 * @param message message from server
 * @return true to continue the message loop, false to exit
 */
bool handle_error_message(client_t* client, const char* message);

#endif // __NETWORK_H