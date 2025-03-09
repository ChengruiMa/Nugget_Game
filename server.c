/* 
* CS50 'Nuggets' Project — Server
* 
* Zachary Rosca-Halmagean, March 2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "message.h"

// function prototypes
static int parseArgs(int argc, char* argv[], int* storedSeed, char* map);
static game_t* initGame(FILE* map);
bool handleMessage(void* arg, const addr_t from, const char* message);
static void endGate(game_t* game);

// structs
typedef struct game {
    grid_t* grid; // master grid
    player_t** players; // list of players
    spectator_t* spectator; // the single spectator
    int playersSeen; // total number of players seen (i.e., both joined and left)
    gold_t* gold;
} game_t;

typedef struct gold {
    int* counter;
    int piles; // num of piles
    int index; // where
} gold_t;

// global variables
const int maxPlayers = 4;
const int maxSpectators = 1;

static int
parseArgs(int argc, char* argv[], int* storedSeed, char* map)
{
    // check for exactly one or two parameters (2 or 3 args including the program name)
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: ./server map.txt [seed]\n");
        return 1; // return 1 to indicate incorrect usage
    }

    if (argc == 3) { // optional parameter (seed) provided, make sure it's in correct format (unsigned integer) & store it
        int seed = atoi(argv[2]);
        if (seed < 0) {
            fprintf(stderr, "Incorrect seed format, must be unsigned integer: %d\n", seed);
            return 3; // return 3 to indicate incorrect seed format
        }

        // store seed
        *storedSeed = seed;
    }

    // check if map file exists
    FILE* mapFile = fopen(argv[1], "r");
    if (mapFile == NULL) {
        fprintf(stderr, "Incorrect map filepath, could not read file: %s\n", argv[1]);
        return 2; // return 2 to indicate map file not found
    }
    // close file
    fclose(mapFile);

    // store map file path
    strcpy(map, argv[1]); // TODO: check if this handles memory correctly

    return 0; // return 0 to indicate successful parsing
}

/**
* Initializes game state
* 
* pretty sure this should create/return a game state struct (or pointer to one) — need to define this struct in the gamestate module's header file
*
* Inputs:
* @param map: a pointer to the opened map FILE with map information to be read
*/
game_t*
initGame(FILE* map) 
{
    if (map == NULL) {
        fprintf(stderr, "Error reading map file\n");
        return 4; // return 4 to indicate error reading map file
    }

    // create game state here?

    // drop gold across map in various piles (at least GoldMinNumPiles and at most GoldMaxNumPiles); random number of nuggets in each (this is handled in the gold module i'm p sure)
}

static void
endGame(game_t* game)
{
    if (game == NULL) {
        fprintf(stderr, "Error freeing game state\n");
        return;
    } // else do below

    // free all memory allocated for game state
    // free grid
    // free players
    // free spectator
    // free gold

    // wrap above cleanup in a function in gamestate module to be called here (it's cleaner)
}

/**
* Converts a message string into an array of strings (each serving a diff purpose)
*
* Inputs:
* @param message: the message string to be parsed
*/

char**
parseMessage(char* message)
{
    // parse message into an array of strings
    // return array of strings

    if (message == NULL) {
        fprintf(stderr, "Error parsing message\n");
        return NULL;
    }

    // split message into array of strings
    char** parsed = malloc(sizeof(char*) * strlen(message)); // p sure this will allocate length of msg and not num of words — don't think it rlly matters tho

    if (parsed == NULL) {
        fprintf(stderr, "Error parsing message\n");
        return NULL;
    }

    char* token = strtok(message, " ");
    int i = 0;

    while (token != NULL) {
        parsed[i] = malloc(sizeof(token) + 1); // +1 for null terminator
        if (parsed[i] == NULL) {
            fprintf(stderr, "Error parsing message\n");
            return NULL;
        }
        strcpy(parsed[i], token); // copy token into array

        token = strtok(NULL, " "); // get next token
        i++;
    }

    return parsed;
}

/**
* Frees memory allocated for a message array
*
* Inputs:
* @param message: the message array to be freed
*/
void
freeMessage(char** message)
{
    if (message == NULL) {
        fprintf(stderr, "Error freeing message\n");
        return NULL;
    }

    for (int i = 0; message[i] != NULL; i++) {
        free(message[i]);
    }

    free(message); // free array
}

bool
handleMessage(void* arg, const addr_t from, const char* message)
{
    if (arg == NULL || from == NULL || message == NULL) {
        fprintf(stderr, "\033[0;31mFATAL Error handling message: client or game does not exist (probably game, make sure it is initialized!) \033[0m\n");
        return true; // true ends the message loop, as defined in the message module (false does not)
    }

    game_t* game = (game_t*) arg; // cast arg to game state

    char** parsed = parseMessage(message);
    
    if (parsed == NULL) {
        fprintf(stderr, "Error handling message\n");
        return true; // true ends the message loop
    }

    // handle message based on parsed message
    // check if message is from a player or spectator
    // if player, check if they are joining, moving, or leaving
    // if spectator, check if they are joining or leaving

    // get args from parsed message
    int args = 0;
    while (parsed[args] != NULL) {
        args++;
    }

    if (args == 0) {
        // client error, send error message to client rather than stderr
        message_send(from, "Message Error: no arguments provided (malformed message)\n");
        // fprintf(stderr, "Message Error: no arguments provided (malformed message)\n");
        free((char*)message); // need to cast message to free it since it's a const char*
        freeMessage(parsed); // should take care of everything?
        return false; // not fatal, continue message loop
    }

    // switch for commands
    switch (parsed[0]) { // first argument, command passed
        case "PLAY":
            // handle request from new player to join game

            if (args >= 2) {
                // handleNewPlayer(game, from, parsed[1]);

                // do we need to reset/initialize a grid for the player? I think so since player_new needs a grid
            
                int rows = game->grid->rows;
                int cols = game->grid->cols;

                char playerLetter = parsed[1][0]; // get first letter of player name
            } else {
                handleMalformedMessage(from, "Message Error: Incorrect number of arguments for command, 'PLAY'", message);
            }
        
        case "KEY":
            // handle keystroke from existing player (move or quit)

            if (args != 2) {
                handleMalformedMessage(from, "Message Error: Incorrect number of arguments for command, 'KEY'", message);
            } else {
                handleKeyPress(game, from, parsed[1]);
            }

            break;
        
        case "SPECTATE":
            // handle request from new spectator to join game

            if (args != 1) {
                handleMalformedMessage(from, "Message Error: Incorrect number of arguments for command, 'SPECTATE'", message);
            } else {
                handleNewSpectator(game, from);
            }

            break;
        
        case "QUIT":
            // handle request from player or spectator to leave game

            if (args != 1) {
                handleMalformedMessage(from, "Message Error: Incorrect number of arguments for command, 'QUIT'", message);
            } else {
                handleQuit(game, from); // handleQuit should handle both player and spectator quitting
            }

            break;
    }


}

static void
handleMalformedMessage(const addr_t from, char* error, char* message)
{
    // build error message (error + message)
    char* errorMessage = malloc(sizeof(error) + sizeof(message) + 4); // +1 for null terminator, +1 for new line + 1 for colon + 1 for tab
    if (errorMessage == NULL) {
        fprintf(stderr, "Error handling malformed message\n");
        return;
    }

    strcpy(errorMessage, error);
    strcat(errorMessage, ":\n\t");
    strcat(errorMessage, message);

    // send error message to client
    message_send(from, errorMessage);

    // free error message
    free(errorMessage);
}

int
main(int argc, char* argv[])
{
    // call parseArgs
    int seed = -1;
    char map[100];

    int parseArgsReturnCode = parseArgs(argc, argv, &seed, map);

    if (parseArgsReturnCode != 0) {
        fprintf(stderr, "Exiting server with return code: %d\n", parseArgsReturnCode);
        return parseArgsReturnCode;
    }

    // generate random map using seed phrase if provided
    if (seed != -1) {
        srand(seed);
    } else {
        srand(getpid());
    }

    // load map file
    FILE* mapFile = fopen(map, "r"); // already checked if file is able to be read in parseArgs, but check again
    if (mapFile == NULL) {
        fprintf(stderr, "Error reading map file: %s\n", mapFile);
        return 4; // return 4 to indicate error reading map file
    }

    // initialize game state

    // close map file
    fclose(mapFile);

    // initialize network and announce port number (handled by message module)
    int port = message_init(stderr);
    if (port == 0) {
        fprintf(stderr, "Failed to initialize server. Possibly no open ports?\n");
        return 5; // return 5 to indicate error initializing server
    }

    // wait for messages from clients (start a loop listening for messages)
}