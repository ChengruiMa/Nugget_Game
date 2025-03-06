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

int
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
int
init_game(FILE* map) 
{
    if (map == NULL) {
        fprintf(stderr, "Error reading map file\n");
        return 4; // return 4 to indicate error reading map file
    }

    // create game state here?

    // drop gold across map in various piles (at least GoldMinNumPiles and at most GoldMaxNumPiles); random number of nuggets in each (this is handled in the gold module i'm p sure)
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