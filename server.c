/*
 * CS50 'Nuggets' Project — Server
 *
 * Zachary Rosca-Halmagean, March 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "game/game.h"
#include "support/message.h"
#include "player/player.h"
#include "spectator/spectator.h"
#include "grid/grid.h"

// function prototypes
static int parseArgs(int argc, char *argv[], int *storedSeed, char *map);
static game_t *initGame(FILE *map);
bool handleMessage(void *arg, const addr_t from, const char *message);
static void endGame(game_t *game);
static void handleKeyPress(game_t *game, const addr_t from, char key);
static void handleQuit(game_t *game, addr_t from);
static void handleNewSpectator(game_t *game, addr_t from);
static void updateSpectatorDisplay(game_t *game, spectator_t *spectator);
static void updatePlayerDisplay(game_t *game, player_t *player);
static void updateSpectatorGold(game_t *game);
static void updateAllPlayerGold(game_t *game);
static int getRemainingGold(game_t* game);
static bool isGameOver(game_t *game);
static void handleGameOver(game_t *game);
static void handleMalformedMessage(const addr_t from, char *error, const char *message);
static void sendOK(player_t *player);

// // structs
// typedef struct game
// {
//     grid_t *grid;           // master grid
//     player_t **players;     // list of players
//     spectator_t *spectator; // the single spectator
//     int playersSeen;        // total number of players seen (i.e., both joined and left)
//     gold_t** goldPiles;
//     int numGoldPiles;
// } game_t;

// typedef struct gold {
//     int row;
//     int col;
//     int amount;
//     player_t* collector; // or 'collectedBy'
// }

// global variables
// const int maxPlayers = 4;
// const int maxSpectators = 1;
// const int MaxNameLength = 20;

/**
 * Parses command line arguments
 * 
 * Inputs:
 * @param argc: the number of arguments provided
 * @param argv: the array of arguments provided
 * @param storedSeed: a pointer to the stored seed value
 */
static int parseArgs(int argc, char *argv[], int *storedSeed, char *map)
{
    // check for exactly one or two parameters (2 or 3 args including the program name)
    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Usage: ./server map.txt [seed]\n");
        return 1; // return 1 to indicate incorrect usage
    }

    if (argc == 3)
    { // optional parameter (seed) provided, make sure it's in correct format (unsigned integer) & store it
        int seed = atoi(argv[2]);
        if (seed < 0)
        {
            fprintf(stderr, "Incorrect seed format, must be unsigned integer: %d\n", seed);
            return 3; // return 3 to indicate incorrect seed format
        }

        // store seed
        *storedSeed = seed;
    }

    // check if map file exists
    FILE *mapFile = fopen(argv[1], "r");
    if (mapFile == NULL)
    {
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
initGame(FILE *map)
{
    if (map == NULL)
    {
        fprintf(stderr, "Error reading map file\n");
        return NULL; // return 4 to indicate error reading map file
    }

    // create game state here?
    game_t* game = game_new(map);
    if (game == NULL)
    {
        fprintf(stderr, "Error creating game state\n");
        return NULL;
    }

    return game;
}

/**
 * Ends game state
 *
 * Inputs:
 * @param game: the game state to end
 */
static void endGame(game_t *game)
{
    if (game == NULL)
    {
        fprintf(stderr, "Error: Invalid game state provided in endGame\n");
        return;
    } // else do below

    game_delete(game); // free all memory allocated for game state
}

/**
 * Compares two players by score (i.e., number of nuggets collected)
 *
 * Inputs:
 * @param a: the first player to compare
 * @param b: the second player to compare
 */
static int comparePlayers(const void *a, const void *b)
{
    player_t *playerA = *(player_t **)a;
    player_t *playerB = *(player_t **)b;

    return playerB->purse - playerA->purse; // sort in descending order
}

/**
 * Handles game over operations (i.e., leaderboard, final score, etc.)
 *
 * Inputs:
 * @param game: the game state to handle game over operations for
 */
static void handleGameOver(game_t *game)
{
    if (game == NULL)
    {
        fprintf(stderr, "Error Invalid game state provided in handleGameOver\n");
        return;
    }

    player_t** players = game->players;
    int playersSeen = game->playersSeen;

    // sort players by score (i.e., number of nuggets collected)
    qsort(players, playersSeen, sizeof(player_t*), comparePlayers); // need to define comparePlayers function

    // create leaderboard end game message
    char* leaderboard = malloc(((playersSeen + 1) * (sizeof(char)*MaxNameLength)) + 50); // should be enough for message (add 1 to players seen to account for spectator)
    if (leaderboard == NULL)
    {
        fprintf(stderr, "Error creating leaderboard message\n");
        return;
    }

    strcpy(leaderboard, "QUIT GAME OVER:\n");
    for (int i = 0; i < playersSeen; i++)
    {
        player_t* player = players[i];
        char* playerScore = malloc(sizeof(char) * MaxNameLength + 10); // should be enough for message
        if (playerScore == NULL)
        {
            fprintf(stderr, "Error creating player score message\n");
            return;
        }

        sprintf(playerScore, "%c %10d %s\n", player->playerLetter, player->purse, player->realName); // format message as per REQUIREMENTS spec
        strcat(leaderboard, playerScore);
        free(playerScore); // free player score message
    }

    // send leaderboard message to all players and spectators
    for (int i = 0; i < playersSeen; i++)
    {
        player_t* player = players[i];
        message_send(player->address, leaderboard);
    }

    if (game->spectator != NULL)
    {
        message_send(game->spectator->address, leaderboard);
    }

    free(leaderboard); // free leaderboard message
}

/**
 * Converts a message string into an array of strings (each serving a diff purpose)
 *
 * Inputs:
 * @param message: the message string to be parsed
 */
char**
parseMessage(const char *message)
{
    // parse message into an array of strings
    // return array of strings

    if (message == NULL)
    {
        fprintf(stderr, "Error parsing message\n");
        return NULL;
    }

    char* messageCopy = strdup(message); // +1 for null terminator
    if (messageCopy == NULL)
    {
        fprintf(stderr, "Error parsing message\n");
        return NULL;
    }

    // split message into array of strings
    char **parsed = malloc(sizeof(char *) * strlen(messageCopy)); // p sure this will allocate length of msg and not num of words — don't think it rlly matters tho

    if (parsed == NULL)
    {
        fprintf(stderr, "Error parsing message\n");
        return NULL;
    }

    char *token = strtok(messageCopy, " ");
    int i = 0;

    while (token != NULL)
    {
        parsed[i] = malloc(sizeof(token) + 1); // +1 for null terminator, or malloc(strlen(token) + 1);
        if (parsed[i] == NULL)
        {
            fprintf(stderr, "Error parsing message\n");
            return NULL;
        }
        strcpy(parsed[i], token); // copy token into array

        token = strtok(NULL, " "); // get next token
        i++;
    }

    parsed[i] = NULL; // set last element to NULL to indicate end of array

    free(messageCopy); // free message copy

    return parsed;
}

/**
 * Frees memory allocated for a message array
 *
 * Inputs:
 * @param message: the message array to be freed
 */
void freeMessage(char **message)
{
    if (message == NULL)
    {
        fprintf(stderr, "Error freeing message\n");
        return;
    }

    for (int i = 0; message[i] != NULL; i++)
    {
        free(message[i]);
    }

    free(message); // free array
}

/**
 * Sends OK message to player, as per REQUIREMENTS spec
 * 
 * Inputs:
 * @param player: the player to send the OK message to
 */
void sendOK(player_t *player)
{
    const char* playerLetter = (const char[]){player->playerLetter, '\0'};
    char *message = malloc(sizeof("OK ") + sizeof(playerLetter) + 2); // +1 for null terminator, +1 for new line
    if (message == NULL)
    {
        fprintf(stderr, "Error sending OK message to player\n");
        return;
    }

    strcpy(message, "OK ");
    strcat(message, playerLetter);

    // send OK w/ player letter to player
    message_send(player->address, message);

    free(message);
}

/**
 * Handles new spectator joining game
 * 
 * Inputs:
 * @param game: the game state to handle new spectator joining
 * @param from: the address of the new spectator joining
 */
static void
handleNewSpectator(game_t* game, addr_t from)
{
    if (game == NULL) {
        fprintf(stderr, "Invalid game state passed to handleNewSpectator\n");
        return;
    }

    // add to game state 
    if (!game_addSpectator(game, from)) {
        fprintf(stderr, "Error adding spectator to game state\n");
        // maybe send error message to spectator's client?
        return;
    }

    // send initial grid message to spectator
    int nrows = game->grid->nrows;
    int ncols = game->grid->ncols;

    char* message = malloc(sizeof(char) * 100); // should be enough for message
    if (message == NULL) {
        fprintf(stderr, "Error sending initial grid message to spectator\n");
        return;
    }

    sprintf(message, "GRID %d %d\n", nrows, ncols); // format message as per REQUIREMENTS spec
    message_send(from, message); // send grid message to spectator
    free(message); // free message

    fprintf(stderr, "Spectator joined game!\n");
}

/**
 * Handles player quitting game
 * 
 * Inputs:
 * @param game: the game state to handle player quitting
 * @param from: the address of the player quitting
 */
void handleQuit(game_t *game, addr_t from)
{
    if (game == NULL)
    {
        fprintf(stderr, "Invalid parameters passed to handleQuit\n");
        return;
    }

    // get player from address
    player_t* player = game_getPlayerFromAddress(game, from);
    // player_t *player = NULL;
    // for (int i = 0; i < game->playersSeen; i++)
    // {
    //     player_t *player = game->players[i];
    //     if (message_eqAddr(player->address, from))
    //     {
    //         break; // here you would return the player (AS A METHOD IN GAMESTATE MDOULE @ARAL)
    //     }
    // }

    // handle spectator quit
    if (player == NULL)
    {
        spectator_t *spectator = game->spectator;

        if (spectator == NULL)
        {
            fprintf(stderr, "Error handling quit: could not find player or spectator with matching address in game\n");
            return;
        }

        message_send(from, "QUIT Thanks for watching!");

        spectator_delete(spectator); // should free spectator memory (THIS SHOULD BE A FUNCTION IN SPECTATOR MODULE THAT SHOULD EXIST @NEAL — important for memory management)
        game->spectator = NULL;      // set spectator pointer to NULL (p sure this is best practice)
    }
    else
    {
        player->leftGame = true;
        // don't remove from game state since we'll want to keep player in game state for game over operations (i.e., leaderboard, final score, etc.)
        message_send(from, "QUIT Thanks for playing!"); // send quit msg as defined in REQUIREMENTS spec
    }
}

/**
 * Updates spectator's display with current game state on their client/screen
 * 
 * Inputs:
 * @param game: the game state to update spectator display for
 * @param spectator: the spectator to update display for
 */
static void updateSpectatorDisplay(game_t *game, spectator_t *spectator)
{
    if (game == NULL)
    {
        fprintf(stderr, "Invalid game state passed to updateSpectatorDisplay\n");
        return;
    }

    if (spectator == NULL) {
        fprintf(stderr, "No spectator currently active in game. Skipping their display update.\n");
        return;
    }

    grid_t *grid = game->grid;
    // char* display = grid_toString(grid); // get grid as string
    // NOTE: BELOW IS SUBJECT TO CHANGE DEPENDING ON MARTIN'S IMPLEMENTATION — LIKELY WILL NEED TO CHANGE/TWEAK
    char *display = game_buildDisplayString(game); // build display string (THIS SHOULD BE A FUNCTION IN THE GRID MODULE @MARTIN)
    fprintf(stderr, "Built display string!\n");
    if (display == NULL)
    {
        fprintf(stderr, "Error updating spectator display\n");
        return;
    }

    char *message = malloc((sizeof(char) * strlen(display)) + 9); // +10 for "DISPLAY\n" and null terminator — should fit in message_MaxBytes according to REQUIREMENTS spec
    if (message == NULL)
    {
        fprintf(stderr, "Error updating spectator display\n");
        return;
    }

    strcpy(message, "DISPLAY\n");
    strcat(message, display);

    message_send(spectator->address, message); // send display message to spectator

    free(message);
    free(display);
}

/**
 * Updates player's display with current game state on their client/screen
 * 
 * Inputs:
 * @param game: the game state to update player display for
 * @param player: the player to update display for
 */
static void updatePlayerDisplay(game_t *game, player_t *player)
{
    if (game == NULL || player == NULL)
    {
        fprintf(stderr, "Invalid parameters passed to updatePlayerDisplay\n");
        return;
    }

    if (player->leftGame)
    {
        fprintf(stderr, "Player %c has left game. Skipping their display update.\n", player->playerLetter);
        return;
    }

    grid_t* playerGrid = player->grid; // get player grid
    point_t *playerPos = point_new(player->row, player->col); // create point_t struct for player position, used in calculate step
    grid_calculateVisibility(playerGrid, playerPos); // calculate player grid visibility

    char *display = game_buildPlayerDisplayString(game, player); // build player display string (THIS SHOULD BE A FUNCTION IN THE GRID MODULE @MARTIN)

    char *message = malloc((sizeof(char) * strlen(display)) + 9); // +10 for "DISPLAY\n" and null terminator — should fit in message_MaxBytes according to REQUIREMENTS spec
    if (message == NULL)
    {
        fprintf(stderr, "Error updating player display\n");
        return;
    }

    strcpy(message, "DISPLAY\n");
    strcat(message, display);

    message_send(player->address, message); // send display message to player

    free(message);
    free(display);
    // visibility_delete(visibility); // remove once grid/visibility is refactored to keep visibility contained in grid/visibility operations — should not be exposed to users of the grid module
    point_delete(playerPos);       // remove once grid/visibility is refactored to keep visibility contained in grid/visibility operations — should not be exposed to users of the grid module
}

/**
 * Updates spectator's gold display by sending them a message with the current gold state
 * 
 * Inputs:
 * @param game: the game state to update spectator gold display for
 */
static void updateSpectatorGold(game_t* game)
{
    spectator_t* spectator = game->spectator;
    if (spectator == NULL)
    {
        fprintf(stderr, "No spectator currently active in game. Skipping their gold update.\n");
        return;
    }

    int remainingGold = getRemainingGold(game);
    // int totalGold = game->gold->total;

    char* message = malloc(sizeof(char) * 50); // should be enough for message
    if (message == NULL)
    {
        fprintf(stderr, "Error updating spectator gold\n");
        return;
    }

    // sprintf(message, "Spectator: %d/%d nuggets unclaimed", remainingGold, totalGold); // need to format / add colors / add bold/emphasis to message later
    
    sprintf(message, "GOLD %d %d %d", 0, 0, remainingGold); // format message as per REQUIREMENTS spec
    message_send(spectator->address, message); // send gold message to spectator
    free(message); // free message
}

/**
 * Updates all player's gold display by sending them a message with the current gold state
 * 
 * Inputs:
 * @param game: the game state to update all player gold display for
 */
static void updateAllPlayerGold(game_t* game)
{
    player_t** players = game->players;
    int playersSeen = game->playersSeen;

    for (int i = 0; i < playersSeen; i++)
    {
        player_t* player = players[i];
        if (player == NULL)
        {
            fprintf(stderr, "Error updating player gold: player is NULL\n");
            continue;
        }

        int currentGold = player->purse;
        int remainingGold = getRemainingGold(game);

        char* message = malloc(sizeof(char) * 50); // should be enough for message
        if (message == NULL)
        {
            fprintf(stderr, "Error updating player gold\n");
            return;
        }

        sprintf(message, "GOLD %d %d %d", 0, currentGold, remainingGold); // format message as per REQUIREMENTS spec
        message_send(player->address, message); // send gold message to player
        free(message); // free message
    }
}

/**
 * Helper function to get the total remaining gold in the game
 * 
 * Inputs:
 * @param game: the game state to get the remaining gold from
 */
static int getRemainingGold(game_t* game)
{
    gold_t** piles = game->goldPiles;
    if (game == NULL || piles == NULL)
    {
        fprintf(stderr, "Error getting remaining gold\n");
        return -1;
    }

    int totalRemaining = 0;
    for (int i = 0; i < game->numPiles; i++)
    {
        totalRemaining += piles[i]->amount;
    }

    return totalRemaining;
}

/**
 * Helper function to check if the game is over (i.e., all gold has been collected)
 * 
 * Inputs:
 * @param game: the game state to check if game is over
 */
static bool isGameOver(game_t* game)
{
    if (game == NULL)
    {
        fprintf(stderr, "Error checking if game is over\n");
        return false;
    }

    int remainingGold = getRemainingGold(game);
    if (remainingGold == 0)
    {
        return true;
    }

    return false;
}

void sendGoldMessage(game_t* game, player_t* player, int goldCollected) {
    if (game == NULL || player == NULL) {
        fprintf(stderr, "Error sending gold message\n");
        return;
    }

    int currentGold = player->purse;
    int remainingGold = getRemainingGold(game);

    char* message = malloc(sizeof(char) * 50); // should be enough for message
    if (message == NULL) {
        fprintf(stderr, "Error sending gold message\n");
        return;
    }

    sprintf(message, "GOLD %d %d %d", goldCollected, currentGold, remainingGold); // format message as per REQUIREMENTS spec
    message_send(player->address, message); // send gold message to player
    free(message); // free message

    // do we also need to update all player gold here? (i.e., call updateAllPlayerGold and updateSpectatorGold — or is this handled every time in the handleKeyPress function anyways?)
    // answer: yes we do, b/c imagine a player toggled right move and the switch never leaves / doesn't unfire, then the gold message would never be sent to other clients
    updateAllPlayerGold(game);
    updateSpectatorGold(game);
}

void movePlayer(game_t* game, player_t* player, int newRow, int newCol)
{
    if (game == NULL || player == NULL)
    {
        fprintf(stderr, "Error moving player: game or player is NULL\n");
        return;
    }

    grid_t* grid = game->grid;

    // check if new position is valid
    if (!(newRow >= 0 && newRow < grid->nrows && newCol >= 0 && newCol < grid->ncols))
    {
        fprintf(stderr, "Error moving player: new position is invalid (exceeds bounds of map)\n");
        return;
    }

    // check if new position is a room
    if (!grid_isRoom(grid, newRow, newCol) && !grid_isPassage(grid, newRow, newCol))
    {
        fprintf(stderr, "Error moving player: new position is not a valid spot on the map (perhaps on a wall or in the void)\n");
        return;
    }

    grid_t* playerGrid = player->grid;

    player_t** players = game->players;
    int playersSeen = game->playersSeen;

    // check if new spot is occupied by another player
    player_t* occupyingPlayer = NULL;
    for (int i = 0; i < playersSeen; i++) {
        occupyingPlayer = players[i];
        if (occupyingPlayer->row == newRow && occupyingPlayer->col == newCol) {
            break; // leaves loop if player is found at position
        }

        occupyingPlayer = NULL; // else set to NULL and continue
    }

    if (occupyingPlayer != NULL) {
        // swap positions with occupying player
        grid_t* occupyingPlayerGrid = occupyingPlayer->grid;
        int tempRow = player->row;
        int tempCol = player->col;

        occupyingPlayer->row = tempRow;
        occupyingPlayer->col = tempCol;

        player->row = newRow;
        player->col = newCol;

        // do i also need to update the grid characters here? (i.e., set player's old position to '.' and new position to player letter)
        // update grid characters  
        // grid_set(grid, tempRow, tempCol, '.');
        // grid_set(grid, newRow, newCol, player->letter);

        // update grid visibility for both players (p sure this is done in updatePlayerDisplay so remove in a sec)
        // grid_calculateVisibility(playerGrid, player->row, player->col);
        // grid_calculateVisibility(occupyingPlayerGrid, occupyingPlayer->row, occupyingPlayer->col);

        // return true;
    } else if (grid_isGold(grid, newRow, newCol)) {
        // update grid characters
        int oldRow = player->row;
        int oldCol = player->col;
        grid_set(grid, oldRow, oldCol, '.');
        grid_set(playerGrid, oldRow, oldCol, '.');

        player->row = newRow;
        player->col = newCol;

        int goldCollected = game_collectGold(game, player); // this should cover the below lines (that are commented out — TODO: SHOULD BE TESTED)
        // gold_t* goldPile = game_getGoldAtPosition(game, newRow, newCol);
        // if (goldPile != NULL) {

        //     player_addGold(player, goldPile->amount); // this should be part of the `game_collectGold` function in game module @ARAL
        //     goldPile->amount = 0; // this should be part of the `game_collectGold` function in game module @ARAL
        //     goldPile->collector = player; // this should be part of the `game_collectGold` function in game module @ARAL
        // }

        // update grid characters
        grid_set(grid, newRow, newCol, '.'); // player letters / stuff are added in the build display string
        grid_set(playerGrid, newRow, newCol, '.'); // player letters / stuff are added in the build display string

        // return true;

        if (goldCollected != -1) {
            sendGoldMessage(game, player, goldCollected);
        }
    } else {
        // update grid characters
        int oldRow = player->row;
        int oldCol = player->col;

        if (!grid_isPassage(grid, oldRow, oldCol)) {
            grid_set(grid, oldRow, oldCol, '.');
            grid_set(playerGrid, oldRow, oldCol, '.');
        } else {
            grid_set(grid, oldRow, oldCol, '#');
            grid_set(playerGrid, oldRow, oldCol, '#');
        }

        if (!grid_isPassage(grid, newRow, newCol)) {
            grid_set(grid, newRow, newCol, '.');
            grid_set(playerGrid, newRow, newCol, '.');
        } else {
            grid_set(grid, newRow, newCol, '#');
            grid_set(playerGrid, newRow, newCol, '#');
        }
        
        player->row = newRow;
        player->col = newCol;

        // return true;
    }

    updatePlayerDisplay(game, player);

    fprintf(stderr, "Player %s moved to %d, %d\n", player->realName, player->row, player->col);
}

void print_addr_t(addr_t addr) {
    char ip_str[INET_ADDRSTRLEN];  // Buffer for IP address

    // Convert binary IP to string
    inet_ntop(AF_INET, &addr.sin_addr, ip_str, sizeof(ip_str));

    // Print IP and port (convert port from network to host byte order)
    fprintf(stderr, "addr_t: %s:%d\n", ip_str, ntohs(addr.sin_port));
}


/**
 * Handles individual key presses from clients (movement and quitting)
 * 
 * Inputs:
 * @param game: the game state to handle key press for
 * @param from: the address of the player sending the key press
 * @param key: the key press sent by the player
 */
static void handleKeyPress(game_t *game, const addr_t from, char key)
{
    // handle key press from player
    // check if key is valid (i.e., one of the arrow keys or 'Q')
    // if valid, move player in that direction
    // if 'Q', remove player from game

    if (game == NULL)
    {
        fprintf(stderr, "Error handling key press\n");
        return;
    }

    // get player from address (THIS SHOULD GO IN THE GAMESTATE MODULE @ARAL)
    player_t* player = game_getPlayerFromAddress(game, from);

    if (player == NULL)
    {
        fprintf(stderr, "Error handling key press: could not find player with matching address in game\n");
        fprintf(stderr, "SERVER PLAYER ADDRESS: ");
        print_addr_t(from);
        // since no player, must be spectator
        // handle spectator quit if key is Q
        if (key == 'Q')
        {
            handleQuit(game, from); // handleQuit should handle both player and spectator quitting
        }
        // else
        // {
        //     // const char* keyString = (const char[]){key, '\0'};
        //     // handleMalformedMessage(from, "Message Error: Invalid key message provided", keyString);
        // }
        return;
    }

    // handle key press for player
    switch (key)
    {
    case 'k':
    case 'w':
    case 'W':
        // move player left, if possible
        // TODO: players might want to be updated to playerList, as outline in implementation spec
        movePlayer(game, player, player->row - 1, player->col); // assuming row is x and col is y (TODO: THE NAMING SHOULD BE CHANGED TO X AND Y @NEAL)
        break;
    case 'j':
    case 's':
    case 'S':
        // move player right, if possible
        movePlayer(game, player, player->row + 1, player->col);
        break;
    case 'l':
    case 'd':
    case 'D':
        // move player up, if possible
        movePlayer(game, player, player->row, player->col + 1);
        break;
    case 'h':
    case 'a':
    case 'A':
        // move player down, if possible
        movePlayer(game, player, player->row, player->col - 1);
        break;
    case 'u':
        // move player up and left, if possible
        movePlayer(game, player, player->row - 1, player->col + 1);
        break;
    case 'n':
        // move player up and right, if possible
        movePlayer(game, player, player->row + 1, player->col + 1);
        break;
    case 'y':
        // move player down and left, if possible
        movePlayer(game, player, player->row - 1, player->col - 1);
        break;
    case 'b':
        // move player down and right, if possible
        movePlayer(game, player, player->row + 1, player->col - 1);
        break;
    // now the uppercase versions of above, which act as a toggle (i.e., while loop to move in that direction until player runs into wall)
    case 'K':
        while (grid_isRoom(game->grid, player->row - 1, player->col) || grid_isPassage(game->grid, player->row - 1, player->col))
        { // pretty sure `grid_isRoom` is basically isEmpty (anything that isn't a wall)
            movePlayer(game, player, player->row - 1, player->col);
        }
        break;
    case 'J':
        while (grid_isRoom(game->grid, player->row + 1, player->col) || grid_isPassage(game->grid, player->row + 1, player->col))
        {
            movePlayer(game, player, player->row + 1, player->col);
        }
        break;
    case 'L':
        while (grid_isRoom(game->grid, player->row, player->col + 1) || grid_isPassage(game->grid, player->row, player->col + 1))
        {
            movePlayer(game, player, player->row, player->col + 1);
        }
        break;
    case 'H':
        while (grid_isRoom(game->grid, player->row, player->col - 1) || grid_isPassage(game->grid, player->row, player->col - 1))
        {
            movePlayer(game, player, player->row, player->col - 1);
        }
        break;
    case 'U':
        while (grid_isRoom(game->grid, player->row - 1, player->col + 1) || grid_isPassage(game->grid, player->row - 1, player->col + 1))
        {
            movePlayer(game, player, player->row - 1, player->col + 1);
        }
        break;
    case 'N':
        while (grid_isRoom(game->grid, player->row + 1, player->col + 1) || grid_isPassage(game->grid, player->row + 1, player->col + 1))
        {
            movePlayer(game, player, player->row + 1, player->col + 1);
        }
        break;
    case 'Y':
        while (grid_isRoom(game->grid, player->row - 1, player->col - 1) || grid_isPassage(game->grid, player->row - 1, player->col - 1))
        {
            movePlayer(game, player, player->row - 1, player->col - 1);
        }
        break;
    case 'B':
        while (grid_isRoom(game->grid, player->row + 1, player->col - 1) || grid_isPassage(game->grid, player->row + 1, player->col - 1))
        {
            movePlayer(game, player, player->row + 1, player->col - 1);
        }
        break;
    case 'Q':
        // remove player from game
        handleQuit(game, from);
        break;
    default:
        break; // not sending error messages for invalid keys, just ignoring them (think of behavior in a normal video game)
    }
}

/**
 * Handles and parses incoming messages from clients
 * 
 * Inputs:
 * @param arg: the game state to handle incoming messages for
 * @param from: the address of the client sending the message
 * @param message: the message string sent by the client
 */
bool handleMessage(void *arg, const addr_t from, const char *message)
{
    if (arg == NULL || message == NULL)
    {
        fprintf(stderr, "\033[0;31mFATAL Error handling message: client or game does not exist (probably game, make sure it is initialized!) \033[0m\n");
        return true; // true ends the message loop, as defined in the message module (false does not)
    }

    game_t *game = (game_t *)arg; // cast arg to game state

    char **parsed = parseMessage(message);

    if (parsed == NULL)
    {
        fprintf(stderr, "Error handling message\n");
        return true; // true ends the message loop
    }

    // handle message based on parsed message
    // check if message is from a player or spectator
    // if player, check if they are joining, moving, or leaving
    // if spectator, check if they are joining or leaving

    // get args from parsed message
    int args = 0;
    while (parsed[args] != NULL)
    {
        args++;
    }

    if (args == 0)
    {
        // client error, send error message to client rather than stderr
        message_send(from, "Message Error: no arguments provided (malformed message)\n");
        // fprintf(stderr, "Message Error: no arguments provided (malformed message)\n");
        free((char *)message); // need to cast message to free it since it's a const char*
        freeMessage(parsed);   // should take care of everything?
        return false;          // not fatal, continue message loop
    }

    // switch for commands
    // switch (parsed[0]) { // first argument, command passed
    if (strcmp(parsed[0], "PLAY") == 0)
    {
        // handle request from new player to join game

        if (args >= 2)
        {
            // handleNewPlayer(game, from, parsed[1]);

            // do we need to reset/initialize a grid for the player? I think so since player_new needs a grid

            int rows = game->grid->nrows;
            int cols = game->grid->ncols;

            // BELOW CREATES PLAYER LETTER
            char playerLetter = 'A' + game->playersSeen; // get player letter based on number of players seen (alphabet is contiguous with ASCII character set, so we can do this)

            // make player name
            char* realName = malloc(sizeof(char) * MaxNameLength); // should be enough for player name
            if (realName == NULL)
            {
                fprintf(stderr, "Memory error while creating player name\n");
                return false; // not fatal, continue message loop
            }
            strcpy(realName, parsed[1]); // copy player name from parsed message

            // generate random starting position for player (handled by player_new according to IMPLEMENTATION SPEC, so commented out for now)
            // point_t* start = grid_findEmptySpot(game->grid); // find empty spot on grid for player to start
            // int x = start->x;
            // int y = start->y;

            if (game->playersSeen >= MaxPlayers)
            {
                // send error message to client
                handleMalformedMessage(from, "Message Error: Maximum number of players reached", message);
                return false; // not fatal, continue message loop
            }

            // create new player
            player_t *newPlayer = player_new(realName, playerLetter, from, game->grid); // didn't see playerLetter being created in the spec's pseudocode?


            if (newPlayer == NULL)
            {
                // delete grid created for player if we created a grid above
                // free(start); // free start point
                // free(realName); // free real name

                handleMalformedMessage(from, "Message Error: Invalid player message provided", message);
            }
            else
            {
                // add player to game state (THIS SHOULD BE A FUNCTION IN THE GAMESTATE MODULE @ARAL)
                // game->players[game->playersSeen] = newPlayer;
                // game->playersSeen++;

                if (!game_addPlayer(game, newPlayer))
                {
                    fprintf(stderr, "Maximum number of players reached\n");
                    return false; // not fatal; true ends the message loop
                }


                // immediately send new client the size of the grid in the format "GRID rows cols"
                char *gridMessage = malloc(sizeof("GRID ") + sizeof(rows) + sizeof(cols) + 2); // +1 for null terminator, +1 for space
                if (gridMessage == NULL)
                {
                    fprintf(stderr, "Error sending grid size to new player\n");
                    return false; // true ends the message loop
                }

                sprintf(gridMessage, "GRID %d %d", rows, cols); // format message as per REQUIREMENTS spec

                message_send(from, gridMessage);
                sendOK(newPlayer); // send OK message to new player
            }
        }
        else
        {
            handleMalformedMessage(from, "Message Error: Incorrect number of arguments for command, 'PLAY'", message);
        }
    }
    else if (strcmp(parsed[0], "KEY") == 0)
    {
        // handle keystroke from existing player (move or quit)

        if (args != 2)
        {
            handleMalformedMessage(from, "Message Error: Incorrect number of arguments for command, 'KEY'", message);
        }
        else
        {
            // make sure key is a single character
            if (strlen(parsed[1]) != 1)
            {
                handleMalformedMessage(from, "Message Error: Invalid key provided", message);
            }
            else
            {
                handleKeyPress(game, from, parsed[1][0]);
            }
        }
    }
    else if (strcmp(parsed[0], "SPECTATE") == 0)
    {
        // handle request from new spectator to join game

        if (args != 1)
        {
            handleMalformedMessage(from, "Message Error: Incorrect number of arguments for command, 'SPECTATE'", message);
        }
        else
        {
            handleNewSpectator(game, from);
        }
    }
    else if (strcmp(parsed[0], "QUIT") == 0)
    {
        // handle request from player or spectator to leave game

        if (args != 1)
        {
            handleMalformedMessage(from, "Message Error: Incorrect number of arguments for command, 'QUIT'", message);
        }
        else
        {
            handleQuit(game, from); // handleQuit should handle both player and spectator quitting
        }
    } // ignore any invalid command (beyond above commands, as per REQUIREMENTS spec)

    // free parsed message
    freeMessage(parsed);

    fprintf(stderr, "Message handled successfully\n");

    // update game state for spectator
    updateSpectatorDisplay(game, game->spectator);

    fprintf(stderr, "Updated spectator display\n");

    // send updated game state to all players and spectators
    int playersSeen = game->playersSeen;
    for (int i = 0; i < playersSeen; i++)
    {

        updatePlayerDisplay(game, game->players[i]);
    }

    fprintf(stderr, "Updated player displays\n");

    // send updated golds to all players and spectators
    updateSpectatorGold(game); // maybe condense these two functions into one? thought they'd be more distinct, but maybe not after writing them
    fprintf(stderr, "Updated spectator gold\n");
    updateAllPlayerGold(game);
    fprintf(stderr, "Updated all player gold\n");

    // check if game is over (i.e., all gold has been collected)
    if (isGameOver(game))
    {
        // handle game over (leaderboards, end game state, etc.)
        handleGameOver(game);
        fprintf(stderr, "Game over\n");
        return true; // true ends the message loop
    } else {
        fprintf(stderr, "Game not over\n");
        return false; // false continues the message loop
    }
}

/**
 * Handles malformed messages by sending an error message to the client
 * 
 * Inputs:
 * @param from: the address of the client to send the error message to
 * @param error: the error message to send
 * @param message: the original message that was malformed
 */
static void handleMalformedMessage(const addr_t from, char *error, const char *message)
{
    // build error message (error + message)
    char *errorMessage = malloc(sizeof(error) + sizeof(message) + 4); // +1 for null terminator, +1 for new line + 1 for colon + 1 for tab
    if (errorMessage == NULL)
    {
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

/**
 * Main function for server
 * 
 * Inputs:
 * @param argc: the number of arguments provided
 * @param argv: the array of arguments provided
 */
int main(int argc, char *argv[])
{
    // call parseArgs
    int seed = -1;
    char map[100];

    int parseArgsReturnCode = parseArgs(argc, argv, &seed, map);

    if (parseArgsReturnCode != 0)
    {
        fprintf(stderr, "Exiting server with return code: %d\n", parseArgsReturnCode);
        return parseArgsReturnCode;
    }

    // generate random map using seed phrase if provided
    if (seed != -1)
    {
        srand(seed);
    }
    else
    {
        srand(getpid());
    }

    // load map file
    FILE *mapFile = fopen(map, "r"); // already checked if file is able to be read in parseArgs, but check again
    if (mapFile == NULL)
    {
        fprintf(stderr, "Error reading map file: %s\n", map);
        return 4; // return 4 to indicate error reading map file
    }

    // initialize game state
    game_t* game = initGame(mapFile);

    // close map file
    fclose(mapFile);

    // initialize network and announce port number (handled by message module)
    int port = message_init(stderr);
    if (port == 0)
    {
        fprintf(stderr, "Failed to initialize server. Possibly no open ports?\n");
        return 5; // return 5 to indicate error initializing server
    }

    // wait for messages from clients (start a loop listening for messages)
    message_loop(
        game, // game state
        0.00, // timeout (0.00 for no timeout — wait indefinitely for messages since players might go afk and come back)
        NULL, // no timeout function
        NULL, // no input function
        handleMessage // handle message function
    );

    // free game state
    endGame(game);

    // print success message on server
    fprintf(stderr, "Server shutting down successfully — thanks for hosting!\n");

    return 0;
}