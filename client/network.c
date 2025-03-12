/*
 * network.c - implementation of network communication module
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../support/message.h"
#include "../support/log.h"
#include "display.h"
#include "network.h"

/*
 * Send a PLAY message to the server
 * @param client client state to send message from
 */
void send_play_message(client_t* client)
{
    if (client == NULL || client->playerName == NULL) {
        return;
    }
    
    char message[message_MaxBytes]; //format message, "PLAY name"
    snprintf(message, sizeof(message), "PLAY %s", client->playerName);
    
    if (!message_send(client->serverAddress, message)) { //send msg
        log_v("Failed to send PLAY message");
    }
}

/*
 * Send a SPECTATE message to the server
 * @param client client state to send message from
 */
void send_spectate_message(client_t* client)
{
    if (client == NULL) {
        return;
    }
    
    if (!message_send(client->serverAddress, "SPECTATE")) { //send spectat msg
        log_v("Failed to send SPECTATE message");
    }
}

/*
 * Send a KEY message to the server
 * @param client client state to send message from
 * @param key key pressed by the user
 */
void send_key_message(client_t* client, char key)
{
    if (client == NULL) {
        return;
    }
    
    char message[message_MaxBytes]; //format msg, "KEY k"
    snprintf(message, sizeof(message), "KEY %c", key);
    
    if (!message_send(client->serverAddress, message)) { //send msg
        log_v("Failed to send KEY message");
    }
}

/*
 * Handle an OK message from the server
 * @param client client state to update
 * @param message message from server
 * @return true to continue the message loop, false to exit
 */
bool handle_ok_message(client_t* client, const char* message)
{
    if (client == NULL || message == NULL) {
        return true;
    }
    
    char playerLetter;
    if (sscanf(message, "OK %c", &playerLetter) == 1) { //extract player letter
        client->playerLetter = playerLetter;
        log_v("Assigned player letter – %c", playerLetter);
    } else {
        log_v("Invalid OK message – %s", message);
    }
    
    return true;
}

/*
 * Handle a GRID message from the server
 * @param client client state to update
 * @param message message from server
 * @return true to continue the message loop, false to exit
 */
bool handle_grid_message(client_t* client, const char* message)
{
    if (client == NULL || message == NULL) {
        return true;
    }
    
    int nrows, ncols;
    if (sscanf(message, "GRID %d %d", &nrows, &ncols) == 2) { //extract grid dimensions
        log_v("Grid dimensions – %d rows, %d columns", nrows, ncols);
        
        if (!client_update_grid(client, nrows, ncols)) { //update grid
            log_v("Failed to update grid");
            return false;
        }
        
        if (!display_initialize(client)) { //initialize display
            log_v("Failed to initialize display");
            return false;
        }
        
        display_status(client, NULL); //update status line
    } else {
        log_v("Invalid GRID message – %s", message);
    }
    
    return true;
}

/*
 * Handle a GOLD message from the server
 * @param client client state to update
 * @param message message from server
 * @return true to continue the message loop, false to exit
 */
bool handle_gold_message(client_t* client, const char* message)
{
    if (client == NULL || message == NULL) {
        return true;
    }
    
    //extract gold values
    int collected, purse, remaining;
    if (sscanf(message, "GOLD %d %d %d", &collected, &purse, &remaining) == 3) {
        client_update_gold(client, collected, purse, remaining); //update gold info
        if (collected > 0) { //show gold update in status line
            char goldMessage[32];
            snprintf(goldMessage, sizeof(goldMessage), "GOLD received: %d", collected);
            display_status(client, goldMessage);
        } else {
            display_status(client, NULL);
        }
    } else {
        log_v("Invalid GOLD message: %s", message);
    }
    
    return true;
}

/*
 * Handle a DISPLAY message from the server
 * @param client client state to update
 * @param message message from server
 * @return true to continue the message loop, false to exit
 */
bool handle_display_message(client_t* client, const char* message)
{
    if (client == NULL || message == NULL) {
        return true;
    }
    
    display_grid(client, message); //display grid
    
    return true;
}

/*
 * Handle a QUIT message from the server
 * @param client client state to update
 * @param message message from server
 * @return false to exit the message loop
 */
bool handle_quit_message(client_t* client, const char* message)
{
    if (client == NULL || message == NULL) {
        return false;
    }
    
    const char* reason;
    if (strncmp(message, "QUIT ", 5) == 0) { //extract quit reason
        reason = message + 5;  // Skip "QUIT "
    } else if (strncmp(message, "QUIT\n", 5) == 0) {
        reason = message + 5;  // Skip "QUIT\n"
    } else {
        reason = "Unknown reason";
    }
    
    client_handle_quit(client, reason); //notify client
    
    display_cleanup(); //clean up display
    
    printf("%s\n", reason); //print quit message
    
    return false; //stop the message loop
}

/*
 * Handle an ERROR message from the server
 * @param client client state to update
 * @param message message from server
 * @return true to continue the message loop, false to exit
 */
bool handle_error_message(client_t* client, const char* message)
{
    if (client == NULL || message == NULL) {
        return true;
    }
    
    const char* errorMsg;
    if (strncmp(message, "ERROR ", 6) == 0) {
        errorMsg = message + 6;  //skip error msg
    } else {
        errorMsg = "Unknown error";
    }
    
    log_v("Server error: %s", errorMsg); //log error
    
    display_status(client, errorMsg); //display error msg
    
    return true;
}