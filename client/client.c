/*
 * client.c - main client module for CS50 Nuggets game
 * 
 * usage: ./client hostname port [playername]
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../support/message.h"
#include "../support/log.h"
#include "client_state.h"
#include "display.h"
#include "network.h"

//global game state
static client_t *game;

//func prototypes
static int parse_args(const int argc, char *argv[]);
static bool handle_input(void *arg);
static bool handle_message(void *arg, const addr_t from, const char *message);
static void cleanup(void);

/* 
 * Main function for the client
 */
int main(int argc, char *argv[])
{
    log_init(stderr); //initialize module
    
    int result = parse_args(argc, argv); //parse args
    if (result != 0) {
        return result;
    }
    
    game = client_new(argv[1], atoi(argv[2]), argc == 4 ? argv[3] : NULL); //init client state
    if (game == NULL) {
        log_v("Failed to initialize client state");
        return 3;
    }
    
    char log_buffer[100];
    snprintf(log_buffer, sizeof(log_buffer), "Initializing network connection to %s:%s", argv[1], argv[2]);
    log_v(log_buffer); //init network
    FILE* output = fopen("client.log", "w");
    int port = message_init(output); //init message module
    if (port == 0) {
        log_v("Failed to initialize message module");
        client_delete(game);
        return 4;
    }
    
    if (game->isSpectator) {
        log_v("Joining as spectator");
        send_spectate_message(game);
    } else {
        snprintf(log_buffer, sizeof(log_buffer), "Joining as player: %s", game->playerName);
        log_v(log_buffer);
        send_play_message(game);
    }
    
    log_v("Initializing display"); //init display
    
    bool loop_result = message_loop(game, 0, NULL, handle_input, handle_message); //main message loop

    fclose(output);
    
    cleanup(); //clean up
    
    return loop_result ? 0 : 5; //return result, 0 if success, 5 if failure
}

/*  
 * Parse and validate command-line arguments
 * @param argc number of command-line arguments
 * @param argv command-line arguments
 * @return 0 if successful, non-zero if error
 */
static int parse_args(const int argc, char *argv[])
{
    if (argc < 3 || argc > 4) { //check number of args
        fprintf(stderr, "Usage: %s hostname port [playername]\n", argv[0]);
        return 1;
    }
    
    char *endptr;
    int port = strtol(argv[2], &endptr, 10);
    if (*endptr != '\0' || port < 1024 || port > 65535) {
        fprintf(stderr, "Error: port must be a number between 1024 and 65535\n");
        return 2;
    }
    
    return 0;
}

/*
 * Handle keyboard input from the user
 * @param arg pointer to client state
 * @return true to continue the loop, false to exit
 */
static bool handle_input(void *arg)
{
    int key = getchar();
    
    if (key == EOF) { //check for EOF
        send_key_message(game, 'Q');
        return true;
    }
    
    if (game->isSpectator) { //check if spectator
        if (key == 'Q') {
            send_key_message(game, key);
        } else {
            display_status(game, "Spectators can only use Q to quit");
        }
    } else { //player has more options
        if (key == 'Q' || key == 'q' || 
            key == 'h' || key == 'H' ||
            key == 'j' || key == 'J' ||
            key == 'k' || key == 'K' ||
            key == 'l' || key == 'L' ||
            key == 'y' || key == 'Y' ||
            key == 'u' || key == 'U' ||
            key == 'b' || key == 'B' ||
            key == 'n' || key == 'N') {
            send_key_message(game, key);
        } 
        // else {
        //     // display_status(game, "Unknown keystroke");
        //     // do nothing
        // }
    }
    
    return false;
}

/*
 * Handle incoming messages from the server
 * @param arg pointer to client state
 * @param from server address
 * @param message message from server
 * @return true to continue the loop, false to exit
 */
static bool handle_message(void *arg, const addr_t from, const char *message)
{
    char log_buffer[100];
    
    if (message == NULL) {
        log_v("Received NULL message");
        return false;
    }
    
    if (!message_isAddr(game->serverAddress)) { //save server address if not already saved
        game->serverAddress = from;
    }
    
    if (strncmp(message, "OK ", 3) == 0) { //handle different message types
        return handle_ok_message(game, message);
    } else if (strncmp(message, "GRID ", 5) == 0) {
        return handle_grid_message(game, message);
    } else if (strncmp(message, "GOLD ", 5) == 0) {
        return handle_gold_message(game, message);
    } else if (strncmp(message, "DISPLAY\n", 8) == 0) {
        return handle_display_message(game, message);
    } else if (strncmp(message, "QUIT ", 5) == 0 || strncmp(message, "QUIT\n", 5) == 0) {
        return handle_quit_message(game, message);
    } else if (strncmp(message, "ERROR ", 6) == 0) {
        return handle_error_message(game, message);
    } else {
        // snprintf(log_buffer, sizeof(log_buffer), "Unknown message type: %s", message);
        // log_v(log_buffer);
        return false;
    }
}

/*
 * Clean up resources before exiting
 */
static void cleanup(void)
{
    display_cleanup(game);
    client_delete(game);
    message_done();
}
