/*
 * network.c - implementation of network communication module
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>  // For inet_ntoa()
#include <netinet/in.h> // For sockaddr_in

#include "../support/message.h"
#include "../support/log.h"
#include "display.h"
#include "network.h"


void print_addr_t(addr_t addr) {
    char ip_str[INET_ADDRSTRLEN];  // Buffer for IP address

    // Convert binary IP to string
    inet_ntop(AF_INET, &addr.sin_addr, ip_str, sizeof(ip_str));

    // Print IP and port (convert port from network to host byte order)
    fprintf(stderr, "addr_t: %s:%d\n", ip_str, ntohs(addr.sin_port));
}

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
    
    message_send(client->serverAddress, message); //send msg
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
    
    message_send(client->serverAddress, "SPECTATE"); //send spectat msg
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
    
    message_send(client->serverAddress, message); //send msg
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
        return true; // fatal
    }
    
    char playerLetter;
    if (sscanf(message, "OK %c", &playerLetter) == 1) { //extract player letter
        client->playerLetter = playerLetter;
        char log_buffer[100];
        snprintf(log_buffer, sizeof(log_buffer), "Assigned player letter – %c", playerLetter);
        log_v(log_buffer);
    } else {
        char log_buffer[100];
        snprintf(log_buffer, sizeof(log_buffer), "Invalid OK message – %s", message);
        log_v(log_buffer);
    }
    
    return false;
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
        return true; // fatal error
    }
    
    int nrows, ncols;
    if (sscanf(message, "GRID %d %d", &nrows, &ncols) == 2) { //extract grid dimensions
        char log_buffer[100];
        snprintf(log_buffer, sizeof(log_buffer), "Grid dimensions – %d rows, %d columns", nrows, ncols);
        log_v(log_buffer);
        
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
        char log_buffer[100];
        snprintf(log_buffer, sizeof(log_buffer), "Invalid GRID message – %s", message);
        log_v(log_buffer);
    }
    
    return false;
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
        return true; // fatal error
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
        char log_buffer[100];
        snprintf(log_buffer, sizeof(log_buffer), "Invalid GOLD message: %s", message);
        log_v(log_buffer);
    }
    
    return false;
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
        return true; // fatal error
    }
    
    display_grid(client, message); //display grid
    
    return false;
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
        return true; // fatal error anyway
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
    
    display_cleanup(client); //clean up display
    
    printf("%s\n", reason); //print quit message
    
    return true; //stop the message loop
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
    
    char log_buffer[100];
    snprintf(log_buffer, sizeof(log_buffer), "Server error: %s", errorMsg);
    log_v(log_buffer); //log error
    
    display_status(client, errorMsg); //display error msg
    
    return false;
}
